
#include "haxsdk_unity.h"
#include "haxsdk_gui.h"

static HaxMethod RunManager__ChangeLevel("Assembly-CSharp", "", "RunManager", "ChangeLevel", nullptr);
static HaxMethod SemiFunc__MainCamera("Assembly-CSharp", "", "SemiFunc", "MainCamera", nullptr);
static HaxMethod Camera__WorldToScreenPoint("UnityEngine.CoreModule", "UnityEngine", "Camera", "WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)");
static HaxMethod Screen__get_width("UnityEngine.CoreModule", "UnityEngine", "Screen", "get_width", nullptr);
static System::FieldInfo RunManager__instance("Assembly-CSharp", "", "RunManager", "instance");
static System::FieldInfo EnemyDirector__instance("Assembly-CSharp", "", "EnemyDirector", "instance");
static System::FieldInfo EnemyDirector__enemiesSpawned("Assembly-CSharp", "", "EnemyDirector", "enemiesSpawned");
static System::FieldInfo EnemyParent__Enemy("Assembly-CSharp", "", "EnemyParent", "Enemy");
static System::FieldInfo Aim_State__Default("Assembly-CSharp", "", "Aim/State", "Grab");

struct RunManager : System::Object
{
    static RunManager instance()
    {
        static auto addr = (RunManager*)RunManager__instance.GetStaticAddress();
        return *addr;
    }
};

struct Enemy : Unity::Component
{

};

struct EnemyParent : Unity::Component
{
    Enemy enemy()
    {
        if (!m_ManagedPtr)
            throw System::NullReferenceException::New();

        return *(Enemy*)((char*)m_ManagedPtr + EnemyParent__Enemy.GetFieldOffset());
    }
};

struct EnemyDirector : Unity::Component
{
    static EnemyDirector instance()
    {
        static auto addr = (EnemyDirector*)EnemyDirector__instance.GetStaticAddress();
        return *addr;
    }

    System::List<EnemyParent> enemiesSpawned()
    {
        if (!m_ManagedPtr)
            throw System::NullReferenceException::New();

        static int32_t offset = EnemyDirector__enemiesSpawned.GetFieldOffset();
        return *(System::List<EnemyParent>*)((char*)m_ManagedPtr + offset);
    }
};

struct Aim_State
{
    static int32_t Grab() { int32_t val; Aim_State__Default.GetStaticValue(&val); return val; }
};

static void RenderMenu()
{
    if (HaxWindow window("Window"); window)
    {
        if (ImGui::Button("Enum"))
        {
            printf("Aim/State.Grab = %d\n", Aim_State::Grab());
        }
    }
}

static void DrawEsp()
{
    Unity::Camera camera = SemiFunc__MainCamera.Thunk<Unity::Camera>();
    if (camera.IsNull())
        return;

    EnemyDirector enemyDirector = EnemyDirector::instance();
    if (!enemyDirector.IsNull())
    {
        for (auto& enemyParent : enemyDirector.enemiesSpawned())
        {
            Unity::Vector3 v = enemyParent.enemy().GetTransform().GetPosition();
            Unity::Vector3 screenPos = camera.WorldToScreenPoint(v);
            if (screenPos.z > 0.f)
                ImGui::GetBackgroundDrawList()->AddText(ImVec2(screenPos.x, Unity::Screen::GetHeight() - screenPos.y), 0xFFFF0000, "Enemy");
        }
    }
}

namespace RepoCheat
{
    void Start()
    {
        printf("START\n");
        HaxSdk::AddMenuRender(RenderMenu);
        HaxSdk::AddBackgroundWork(DrawEsp);
    }
}