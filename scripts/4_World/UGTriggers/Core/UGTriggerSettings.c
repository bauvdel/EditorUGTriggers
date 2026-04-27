// Centralized configuration for UG Triggers
class UGTriggerSettings
{
    // Step size for incremental scaling operations
    protected static const float DEFAULT_STEP_SIZE = 1.0;

    // Default trigger dimensions (Length, Height, Width)
    protected static const vector DEFAULT_TRIGGER_SIZE = "10 10 10";

    // Minimum allowed dimension for any axis
    protected static const float MIN_DIMENSION = 0.001;

    // Distance tolerance for position matching during import/export
    protected static const float POSITION_TOLERANCE = 0.1;

    // Timer intervals
    protected static const float SYNC_TIMER_INTERVAL = 0.05;       // 50ms update cycle
    protected static const float UNDO_CACHE_TTL = 90.0;            // 90 second undo history

    // Default accommodation and interpolation values
    protected static const float DEFAULT_OUTER_ACCOMMODATION = 1.0;
    protected static const float DEFAULT_INNER_ACCOMMODATION = 0.0;
    protected static const float DEFAULT_INTERPOLATION = 1.0;

    // Default values for new properties
    protected static const bool DEFAULT_USE_LINE_POINT_FADE = false;
    protected static const string DEFAULT_AMBIENT_SOUND_TYPE = "";
    protected static const string DEFAULT_AMBIENT_SOUND_SET = "";
    protected static const bool DEFAULT_LIGHT_LERP = false;

    // UI notification delays
    protected static const int DIALOG_CLOSE_DELAY = 100;           // milliseconds

    // Trigger types
    protected static const int TYPE_OUTER = 0;
    protected static const int TYPE_INNER = 1;
    protected static const int TYPE_TRANSITIONAL = 2;
    protected static const int TYPE_MIN = 0;
    protected static const int TYPE_MAX = 2;

    // Breadcrumb scanning radius
    protected static const float BREADCRUMB_SCAN_RADIUS = 200.0;

    static float GetStepSize()
    {
        return DEFAULT_STEP_SIZE;
    }

    static vector GetDefaultTriggerSize()
    {
        return DEFAULT_TRIGGER_SIZE;
    }

    static float GetMinDimension()
    {
        return MIN_DIMENSION;
    }

    static float GetPositionTolerance()
    {
        return POSITION_TOLERANCE;
    }

    static float GetSyncTimerInterval()
    {
        return SYNC_TIMER_INTERVAL;
    }

    static float GetUndoCacheTTL()
    {
        return UNDO_CACHE_TTL;
    }

    static float GetDefaultOuterAccommodation()
    {
        return DEFAULT_OUTER_ACCOMMODATION;
    }

    static float GetDefaultInnerAccommodation()
    {
        return DEFAULT_INNER_ACCOMMODATION;
    }

    static float GetDefaultInterpolation()
    {
        return DEFAULT_INTERPOLATION;
    }

    static int GetDialogCloseDelay()
    {
        return DIALOG_CLOSE_DELAY;
    }

    static int GetOuterType() { return TYPE_OUTER; }
    static int GetInnerType() { return TYPE_INNER; }
    static int GetTransitionalType() { return TYPE_TRANSITIONAL; }
    static int GetMinType() { return TYPE_MIN; }
    static int GetMaxType() { return TYPE_MAX; }

    static bool IsValidType(int type)
    {
        return type >= TYPE_MIN && type <= TYPE_MAX;
    }

    static float GetBreadcrumbScanRadius()
    {
        return BREADCRUMB_SCAN_RADIUS;
    }

    static vector ClampSize(vector size)
    {
        vector result = size;
        if (result[0] <= MIN_DIMENSION) result[0] = MIN_DIMENSION;
        if (result[1] <= MIN_DIMENSION) result[1] = MIN_DIMENSION;
        if (result[2] <= MIN_DIMENSION) result[2] = MIN_DIMENSION;
        return result;
    }

    static int ClampType(int type)
    {
        return Math.Clamp(type, TYPE_MIN, TYPE_MAX);
    }

    static bool GetDefaultUseLinePointFade()
    {
        return DEFAULT_USE_LINE_POINT_FADE;
    }

    static string GetDefaultAmbientSoundType()
    {
        return DEFAULT_AMBIENT_SOUND_TYPE;
    }

    static string GetDefaultAmbientSoundSet()
    {
        return DEFAULT_AMBIENT_SOUND_SET;
    }

    static bool GetDefaultLightLerp()
    {
        return DEFAULT_LIGHT_LERP;
    }
}