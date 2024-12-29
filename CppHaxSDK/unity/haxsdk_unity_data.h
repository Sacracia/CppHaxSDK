#ifdef HAXSDK_STATIC_FIELD

#undef HAXSDK_STATIC_FIELD
#endif

#ifdef HAXSDK_FIELD_OFFSET

#undef HAXSDK_FIELD_OFFSET
#endif

#ifdef HAXSDK_FUNCTION
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Vector3, Distance, "System.Single(UnityEngine.Vector3,UnityEngine.Vector3)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, FindObjectsOfType, "UnityEngine.Object[](System.Type)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, FindObjectOfType, "UnityEngine.Object(System.Type)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, Destroy, "System.Void(UnityEngine.Object,System.Single)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, get_name, "System.String()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Component, get_transform, "UnityEngine.Transform()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Component, get_gameObject, "UnityEngine.GameObject()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, get_transform, "UnityEngine.Transform()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, get_activeSelf, "System.Boolean()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, set_layer, "System.Void(System.Int32)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, SetActive, "System.Void(System.Boolean)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, AddComponent, "UnityEngine.Component(System.Type)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, GetComponent, "UnityEngine.Component(System.Type)");
HAXSDK_FUNCTION_EXT(UNITY_CORE_ASSEMBLY, "UnityEngine", GameObject, ctor, ".ctor", "System.Void(System.String)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, get_parent, "UnityEngine.Transform()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, set_parent, "System.Void(UnityEngine.Transform)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, get_forward, "UnityEngine.Vector3()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Camera, get_main, "UnityEngine.Camera()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Screen, get_height, "System.Int32()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Screen, get_width, "System.Int32()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Light, set_intensity, "System.Void(System.Single)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Light, get_intensity, "System.Single()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Light, set_range, "System.Void(System.Single)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Light, get_range, "System.Single()");

#ifdef HAX_MONO
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, INTERNAL_get_position, "System.Void(UnityEngine.Vector3&)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, INTERNAL_set_position, "System.Void(UnityEngine.Vector3&)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Camera, INTERNAL_CALL_WorldToScreenPoint, "System.Void(UnityEngine.Camera,UnityEngine.Vector3&,UnityEngine.Vector3&)");
#else
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, get_position, "UnityEngine.Vector3()");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, set_position, "System.Void(UnityEngine.Vector3)");
HAXSDK_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Camera, WorldToScreenPoint, "UnityEngine.Vector3(UnityEngine.Vector3)");
#endif

#undef HAXSDK_FUNCTION
#undef HAXSDK_FUNCTION_EXT
#endif