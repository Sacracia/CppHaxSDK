#define MONO_ASSEMBLY   "Assembly-CSharp"
#define MONO_NAMESPACE  ""
#define MONO_CLASS      "HeroController"
namespace hero_controller {
    #ifdef MONO_STATIC_FIELD
    namespace static_fields {
        MONO_STATIC_FIELD(_instance, void*);
    }
    #endif

    #ifdef MONO_FIELD_OFFSET
    namespace offsets {
        MONO_FIELD_OFFSET(playerData);
        MONO_FIELD_OFFSET(transform);
    }
    #endif

    #ifdef MONO_GAME_FUNC
    namespace funcs {
        MONO_GAME_FUNC(void, HeroController_AddGeo, (MonoObject* __this, int amount), "AddGeo (int)");
        MONO_GAME_FUNC(void, HeroController_Update, (MonoObject* __this), "Update ()");
    }
    #endif
}
#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE
#undef MONO_CLASS