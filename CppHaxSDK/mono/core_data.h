#ifdef MONO_STATIC_FIELD

#endif

#ifdef MONO_FIELD_OFFSET

#endif

#ifdef MONO_FUNCTION
MONO_FUNCTION("mscorlib", "System", Type, GetType, "(string)");

MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Vector3, Distance, "(UnityEngine.Vector3,UnityEngine.Vector3)");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, FindObjectsOfType, "(System.Type)");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Object, FindObjectOfType, "(System.Type)");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Component, get_transform, "()");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Transform, get_position, "()");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Camera, get_main, "()");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Camera, WorldToScreenPoint, "(UnityEngine.Vector3)");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Screen, get_height, "()");
MONO_FUNCTION(UNITY_CORE_ASSEMBLY, "UnityEngine", Screen, get_width, "()");
#endif