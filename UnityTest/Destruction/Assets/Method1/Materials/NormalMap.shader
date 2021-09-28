Shader "Mac/NormalMap"
{
	Properties
	{
		_Color("Color Tint", Color) = (1.0, 1.0, 1.0, 1.0)
		_MainTex("Diffuse Texture", 2D) = "white" {}
		_BumpMap("Normal Texture", 2D) = "bump" {}
		_BumpDepth("Bump Depth", Range(-2.0, 2.0)) = 1
		_SpecColor("Specular Color", Color) = (1.0, 1.0, 1.0, 1.0)
		_Shininess("Shininess", float) = 10
		_RimColor("Rim Color", Color) = (1.0, 1.0, 1.0, 1.0)
		_RimPower("Rim Power", Range(0.1, 10.0)) = 3.0
		_Alpha("Alpha", Range(0.0, 1.0)) = 1.0 
		_IsDestructed("IsDestructed", Range(0.0, 1.0)) = 0.0
	}

	SubShader
	{
		Pass
		{
			Tags 
			{ 
				"LightMode" = "ForwardBase"
				"Queue" = "Transparent" 
				"RenderType" = "Transparent" 
			}

			Blend SrcAlpha OneMinusSrcAlpha
			// Cull Off
			// ZWrite Off

			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma exclude_renderers flash
	
			// User Defined Variables
			uniform float3 _Color;
			uniform sampler2D _MainTex;
			uniform float4 _MainTex_ST;
			uniform sampler2D _BumpMap;
			uniform float4 _BumpMap_ST;
			uniform float _BumpDepth;
			uniform float4 _SpecColor;
			uniform float _Shininess;
			uniform float4 _RimColor;
			uniform float _RimPower;
			uniform float _Alpha;
			uniform float _IsDestructed;
			uniform float4x4 _Transforms[32];

			uniform float4 _Rotation[32];
			uniform float4 _Translate[32];

			// Unity Defined Variables;
			uniform float4 _LightColor0;
	
			float4x4 getTransform(int groupID)
			{
				return _Transforms[groupID];
				float4x4 m;
 
				float4 rotation = _Rotation[groupID];
				float4 translate = _Translate[groupID];

				float yy2 = 2.0 * rotation.y * rotation.y;
				float zz2 = 2.0 * rotation.z * rotation.z;
				float xy2 = 2.0 * rotation.x * rotation.y;
				float sz2 = 2.0 * rotation.w * rotation.z;
				float xz2 = 2.0 * rotation.x * rotation.z;
				float sy2 = 2.0 * rotation.w * rotation.y;
				float xx2 = 2.0 * rotation.x * rotation.x;
				float yz2 = 2.0 * rotation.y * rotation.z;
				float sx2 = 2.0 * rotation.w * rotation.x;

				m[0] = float4(1.0 - yy2 - zz2, xy2 + sz2, xz2 - sy2, translate.x);
				m[1] = float4(xy2 - sz2, 1.0 - xx2 - zz2, yz2 + sx2, translate.y);
				m[2] = float4(xz2 + sy2, yz2 - sx2, 1.0 - xx2 - yy2, translate.z);
				m[3] = float4(0.0, 0.0, 0.0, 1.0);

				return m;
			}

			// Base Input Structs
			struct vertexInput 
			{
				float4 vertex: POSITION;
				float3 normal: NORMAL;
				float4 texcoord: TEXCOORD0;
				float4 groupID_CrossSection: TEXCOORD2;
				float4 tangent: TANGENT;
			};
		
			struct vertexOutput 
			{
				float4 pos: SV_POSITION;
				float4 tex: TEXCOORD0;
				float4 posWorld: TEXCOORD1;
				float3 normalWorld: TEXCOORD2;
				float3 tangentWorld: TEXCOORD3;
				float3 binormalWorld: TEXCOORD4;
			};

			void demolish(inout vertexInput v, int groupID, int crossSection)
			{
				v.vertex.xyz *= max(_IsDestructed, (1.0f - crossSection)); // if (_IsDestructed==1) || (crossSection==0) vetex not shrink

				// Convert Physics Transform to Object Space
				if (_IsDestructed == 1.0)
				{
					float4x4 transform = mul(unity_WorldToObject, getTransform(groupID));
					v.vertex.xyz = mul(transform, float4(v.vertex.xyz, 1.0));

					v.normal.xyz = mul(transform, float4(v.normal.xyz, 0.0));
					v.tangent.xyz = mul(transform, float4(v.tangent.xyz, 0.0));
				}
			}
	
			// Vertex Function
			vertexOutput vert(vertexInput v) 
			{
				vertexOutput o;

				int groupID = (int)(v.groupID_CrossSection.x);
				int crossSection = (int)(v.groupID_CrossSection.y);

				demolish(v, groupID, crossSection);

				o.posWorld = mul(unity_ObjectToWorld, v.vertex);
				o.normalWorld = normalize(mul(unity_ObjectToWorld, v.normal).xyz);
				o.tangentWorld = normalize(mul(unity_ObjectToWorld, v.tangent).xyz);
				o.binormalWorld = normalize(cross(o.normalWorld, o.tangentWorld));
				
				o.pos = UnityObjectToClipPos(v.vertex);
				o.tex = v.texcoord;
	
				return o;
			}
	
			// Fragment Function
			float4 frag(vertexOutput i) : COLOR 
			{
				float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
				float3 lightDirection;
				float atten;
	
				if (_WorldSpaceLightPos0.w == 0.0) 
				{ 
					// Directional Light
					atten = 1.0;
					lightDirection = normalize(_WorldSpaceLightPos0.xyz);
				}
				else 
				{	
					float3 fragmentToLightSource = _WorldSpaceLightPos0.xyz - i.posWorld.xyz;
					float distance = length(fragmentToLightSource);
					float atten = 1 / distance;
					lightDirection = normalize(fragmentToLightSource);
				}
	
				// Texture Maps
				float4 tex = tex2D(_MainTex, i.tex.xy * _MainTex_ST.xy + _MainTex_ST.zw);
				float4 texN = tex2D(_BumpMap, i.tex.xy * _BumpMap_ST.xy + _BumpMap_ST.zw);
	
				// unpackNormal Function
				float3 localCoords = float3(2.0 * texN.ag - float2(1.0,1.0), 0.0);
				//localCoords.z = 1.0 - 0.5 * dot( localCoords, localCoords );
				//localCoords.z = 1.0;
				localCoords.z = _BumpDepth;
	
				// Normal Transpose Matrix
				float3x3 local2WorldTranspose = float3x3
				(
					i.tangentWorld,
					i.binormalWorld,
					i.normalWorld
				);
	
				// Calculate Normal Direction
				float3 normalDirection = normalize(mul(localCoords, local2WorldTranspose));
	
				// Lighting
				float3 diffuseReflection = atten * _LightColor0.rgb * saturate(dot(normalDirection, lightDirection));
				float3 specularReflection = diffuseReflection * _SpecColor.rgb * pow(saturate(dot(reflect(-lightDirection, normalDirection), viewDirection)), _Shininess);
	
				// Rim Lighting
				float rim = 1 - saturate(dot(viewDirection, normalDirection));
				float3 rimLighting = saturate(pow(rim, _RimPower) * _RimColor.rgb * diffuseReflection);
				float3 lightFinal = diffuseReflection + specularReflection + rimLighting + UNITY_LIGHTMODEL_AMBIENT.rgb;
	
				return float4(tex.xyz * lightFinal * _Color.xyz, _Alpha);
			}
	
			ENDCG
		}

		/*
		Pass
		{
			Tags
			{
				"LightMode" = "ForwardAdd"
				"Queue" = "Transparent"
				"RenderType" = "Transparent"
			}
			Blend One OneMinusSrcAlpha
			// Cull Off
			// ZWrite Off

			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma exclude_renderers flash

			// User Defined Variables
			uniform float3 _Color;
			uniform sampler2D _MainTex;
			uniform float4 _MainTex_ST;
			uniform sampler2D _BumpMap;
			uniform float4 _BumpMap_ST;
			uniform float _BumpDepth;
			uniform float4 _SpecColor;
			uniform float _Shininess;
			uniform float4 _RimColor;
			uniform float _RimPower;
			uniform float _Alpha;
			uniform float _IsDestructed;
			uniform float4x4 _Transforms[16];

			// Unity Defined Variables;
			uniform float4 _LightColor0;
			
			// Base Input Structs
			struct vertexInput 
			{
				float4 vertex: POSITION;
				float3 normal: NORMAL;
				float4 texcoord: TEXCOORD0;
				float4 groupID_CrossSection: TEXCOORD2;
				float4 tangent: TANGENT;
			};

			struct vertexOutput 
			{
				float4 pos: SV_POSITION;
				float4 tex: TEXCOORD0;
				float4 posWorld: TEXCOORD1;
				float3 normalWorld: TEXCOORD2;
				float3 tangentWorld: TEXCOORD3;
				float3 binormalWorld: TEXCOORD4;
			};

			float4x4 getTransform(int groupID)
			{
				float4x4 m;

				float4 rotation = _Rotation[groupID];

				float yy2 = 2.0 * rotation.y * rotation.y;
				float zz2 = 2.0 * rotation.z * rotation.z;
				float xy2 = 2.0 * rotation.x * rotation.y;
				float sz2 = 2.0 * rotation.w * rotation.z;
				float xz2 = 2.0 * rotation.x * rotation.z;
				float sy2 = 2.0 * rotation.w * rotation.y;
				float xx2 = 2.0 * rotation.x * rotation.x;
				float yz2 = 2.0 * rotation.y * rotation.z;
				float sx2 = 2.0 * rotation.w * rotation.x;

				m[0] = float4(1.0 - yy2 - zz2, xy2 - sz2, xz2 + sy2, 0.0);
				m[1] = float4(xy2 + sz2, 1.0 - xx2 - zz2, yz2 - sx2, 0.0);
				m[2] = float4(xz2 - sy2, yz2 + sx2, 1.0 - xx2 - yy2, 0.0);
				m[3] = float4(0.0, 0.0, 0.0, 1.0);

				return m;

				// return _Transforms[groupID]);
			}

			// Vertex Function
			vertexOutput vert(vertexInput v)
			{
				vertexOutput o;

				int groupID = (int)v.groupID_CrossSection.x;
				int crossSection = (int)v.groupID_CrossSection.y;

				v.vertex.xyz *= max(_IsDestructed, (1.0f - crossSection)); // if (_IsDestructed==1) || (crossSection==0) vetex not shrink

				float4x4 transform = mul(unity_WorldToObject, getTransform(groupID));
				if (_IsDestructed == 1.0)
				{
					v.vertex.xyz = mul(transform, float4(v.vertex.xyz, 1.0));
					v.normal.xyz = mul(transform, float4(v.normal.xyz, 0.0));
					v.tangent.xyz = mul(transform, float4(v.tangent.xyz, 0.0));
				}

				o.posWorld = mul(unity_ObjectToWorld, v.vertex);
				o.normalWorld = normalize(mul(unity_ObjectToWorld, v.normal).xyz);
				o.tangentWorld = normalize(mul(unity_ObjectToWorld, v.tangent).xyz);
				o.binormalWorld = normalize(cross(o.normalWorld, o.tangentWorld));

				o.pos = UnityObjectToClipPos(v.vertex);
				o.tex = v.texcoord;

				return o;
			}


			// Fragment Function
			float4 frag(vertexOutput i) : COLOR 
			{
				float3 viewDirection = normalize(_WorldSpaceCameraPos.xyz - i.posWorld.xyz);
				float3 lightDirection;
				float atten;

				if (_WorldSpaceLightPos0.w == 0.0) { // Directional Light
					atten = 1.0;
					lightDirection = normalize(_WorldSpaceLightPos0.xyz);
				}
				else 
				{
					float3 fragmentToLightSource = _WorldSpaceLightPos0.xyz - i.posWorld.xyz;
					float distance = length(fragmentToLightSource);
					float atten = 1 / distance;
					lightDirection = normalize(fragmentToLightSource);
				}

				// Texture Maps
				float4 tex = tex2D(_MainTex, i.tex.xy * _MainTex_ST.xy + _MainTex_ST.zw);
				float4 texN = tex2D(_BumpMap, i.tex.xy * _BumpMap_ST.xy + _BumpMap_ST.zw);

				// unpackNormal Function
				float3 localCoords = float3(2.0 * texN.ag - float2(1.0,1.0), 0.0);
				//localCoords.z = 1.0 - 0.5 * dot( localCoords, localCoords );
				//localCoords.z = 1.0;
				localCoords.z = _BumpDepth;

				// Normal Transpose Matrix
				float3x3 local2WorldTranspose = float3x3(
					i.tangentWorld,
					i.binormalWorld,
					i.normalWorld
				);

				// Calculate Normal Direction
				float3 normalDirection = normalize(mul(localCoords, local2WorldTranspose));

				// Lighting
				float3 diffuseReflection = atten * _LightColor0.rgb * saturate(dot(normalDirection, lightDirection));
				float3 specularReflection = diffuseReflection * _SpecColor.rgb * pow(saturate(dot(reflect(-lightDirection, normalDirection), viewDirection)), _Shininess);

				// Rim Lighting
				float rim = 1 - saturate(dot(viewDirection, normalDirection));
				float3 rimLighting = saturate(pow(rim, _RimPower) * _RimColor.rgb * diffuseReflection);
				float3 lightFinal = diffuseReflection + specularReflection + rimLighting;


				return float4(tex.xyz * lightFinal * _Color.xyz, _Alpha);
			}
			ENDCG
		}

		Pass
		{
			Tags 
			{ 
				"LightMode" = "ShadowCaster" 
			}
			Blend Off

			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma exclude_renderers flash

			// User Defined Variables
			uniform float3 _Color;
			uniform sampler2D _MainTex;
			uniform float4 _MainTex_ST;
			uniform sampler2D _BumpMap;
			uniform float4 _BumpMap_ST;
			uniform float _BumpDepth;
			uniform float4 _SpecColor;
			uniform float _Shininess;
			uniform float4 _RimColor;
			uniform float _RimPower;
			uniform float _Alpha;
			uniform float _IsDestructed;
			uniform float4x4 _Transforms[16];

			// Unity Defined Variables;
			uniform float4 _LightColor0;

			// Base Input Structs
			struct vertexInput
			{
				float4 vertex: POSITION;
				float3 normal: NORMAL;
				float4 texcoord: TEXCOORD0;
				float4 groupID_CrossSection: TEXCOORD2;
			};

			struct vertexOutput
			{
				float4 pos: SV_POSITION;
				float4 posWorld: TEXCOORD1;
			};
			 
			float4x4 getTransform(int groupID)
			{
				float4x4 m;

				float4 rotation = _Rotation[groupID];

				float yy2 = 2.0 * rotation.y * rotation.y;
				float zz2 = 2.0 * rotation.z * rotation.z;
				float xy2 = 2.0 * rotation.x * rotation.y;
				float xz2 = 2.0 * rotation.x * rotation.z;
				float xx2 = 2.0 * rotation.x * rotation.x;
				float yz2 = 2.0 * rotation.y * rotation.z;
				float sz2 = 2.0 * rotation.w * rotation.z;
				float sy2 = 2.0 * rotation.w * rotation.y;
				float sx2 = 2.0 * rotation.w * rotation.x;

				m[0] = float4(1.0 - yy2 - zz2, xy2 - sz2, xz2 + sy2, 0.0);
				m[1] = float4(xy2 + sz2, 1.0 - xx2 - zz2, yz2 - sx2, 0.0);
				m[2] = float4(xz2 - sy2, yz2 + sx2, 1.0 - xx2 - yy2, 0.0);
				m[3] = float4(0.0, 0.0, 0.0, 1.0);

				return m;

				// return _Transforms[groupID]);
			}

			// Vertex Function
			vertexOutput vert(vertexInput v)
			{
				vertexOutput o;

				int groupID = (int)v.groupID_CrossSection.x;
				int crossSection = (int)v.groupID_CrossSection.y;

				v.vertex.xyz *= max(_IsDestructed, (1.0f - crossSection)); // if (_IsDestructed==1) || (crossSection==0) vetex not shrink

				float4x4 transform = mul(unity_WorldToObject, getTransform(groupID));
				if (_IsDestructed == 1.0)
				{	
					v.vertex.xyz = mul(transform, float4(v.vertex.xyz, 1.0));
				}

				o.posWorld = mul(unity_ObjectToWorld, v.vertex);
				o.pos = UnityObjectToClipPos(v.vertex);

				return o;
			}

			// Fragment Function
			float4 frag(vertexOutput i) : COLOR
			{
				return float4(1.0, 1.0, 1.0, _Alpha);
			}
			ENDCG
		}
		*/
	}
}