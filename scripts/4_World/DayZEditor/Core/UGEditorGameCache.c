class UGEditorGameCache
{
    protected static ref UGEditorGameCache s_Instance;
    protected DayZGame m_Game;
    protected UIManager m_UIManager;
    protected ScriptCallQueue m_GUICallQueue;
    protected ScriptCallQueue m_SystemCallQueue;
    protected Editor m_Editor;
    protected bool m_GameCached;
    protected bool m_UIManagerCached;
    protected bool m_GUICallQueueCached;
    protected bool m_SystemCallQueueCached;
    protected bool m_EditorCached;

    protected void UGEditorGameCache()
    {
        InvalidateCache();
    }

    static UGEditorGameCache GetInstance()
    {
        if (!s_Instance)
            s_Instance = new UGEditorGameCache();
        return s_Instance;
    }

    static DayZGame GetCachedGame()
    {
        UGEditorGameCache cache = GetInstance();
        if (!cache.m_GameCached || !cache.m_Game)
        {
            cache.m_Game = GetGame();
            cache.m_GameCached = true;
        }
        return cache.m_Game;
    }

    static UIManager GetCachedUIManager()
    {
        UGEditorGameCache cache = GetInstance();
        if (!cache.m_UIManagerCached || !cache.m_UIManager)
        {
            DayZGame game = GetCachedGame();
            if (game)
            {
                cache.m_UIManager = game.GetUIManager();
                cache.m_UIManagerCached = true;
            }
        }
        return cache.m_UIManager;
    }

    static ScriptCallQueue GetCachedGUICallQueue()
    {
        UGEditorGameCache cache = GetInstance();
        if (!cache.m_GUICallQueueCached || !cache.m_GUICallQueue)
        {
            DayZGame game = GetCachedGame();
            if (game)
            {
                cache.m_GUICallQueue = game.GetCallQueue(CALL_CATEGORY_GUI);
                cache.m_GUICallQueueCached = true;
            }
        }
        return cache.m_GUICallQueue;
    }

    static ScriptCallQueue GetCachedSystemCallQueue()
    {
        UGEditorGameCache cache = GetInstance();
        if (!cache.m_SystemCallQueueCached || !cache.m_SystemCallQueue)
        {
            DayZGame game = GetCachedGame();
            if (game)
            {
                cache.m_SystemCallQueue = game.GetCallQueue(CALL_CATEGORY_SYSTEM);
                cache.m_SystemCallQueueCached = true;
            }
        }
        return cache.m_SystemCallQueue;
    }

    static Editor GetCachedEditor()
    {
        UGEditorGameCache cache = GetInstance();
        if (!cache.m_EditorCached || !cache.m_Editor)
        {
            cache.m_Editor = GetEditor();
            cache.m_EditorCached = true;
        }
        return cache.m_Editor;
    }

    static float GetCachedTime()
    {
        DayZGame game = GetCachedGame();
        if (game)
            return game.GetTime() * 0.001; // Convert to seconds
        return 0.0;
    }

    static void InvalidateCache()
    {
        UGEditorGameCache cache = GetInstance();
        cache.m_Game = null;
        cache.m_UIManager = null;
        cache.m_GUICallQueue = null;
        cache.m_SystemCallQueue = null;
        cache.m_Editor = null;

        cache.m_GameCached = false;
        cache.m_UIManagerCached = false;
        cache.m_GUICallQueueCached = false;
        cache.m_SystemCallQueueCached = false;
        cache.m_EditorCached = false;
    }

    static bool IsCacheValid()
    {
        UGEditorGameCache cache = GetInstance();
        return cache.m_GameCached && cache.m_Game;
    }

    static void RefreshCache()
    {
        InvalidateCache();
        GetCachedGame();
        GetCachedUIManager();
        GetCachedGUICallQueue();
        GetCachedSystemCallQueue();
    }

    static void Cleanup()
    {
        if (s_Instance)
        {
            s_Instance.InvalidateCache();
            s_Instance = null;
        }
    }
}