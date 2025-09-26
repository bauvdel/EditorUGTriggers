enum eUGTriggerType
{
    OUTER = 0,         // Outer trigger - high eye accommodation
    INNER = 1,         // Inner trigger - low eye accommodation
    TRANSITIONAL = 2   // Transitional trigger - supports breadcrumbs
}

// Export operation results
enum eUGTriggerExportResult
{
    SUCCESS = 0,           // Export completed successfully
    EDITOR_NOT_FOUND = 1,  // Editor instance not available
    NO_OBJECTS = 2,        // No objects to export
    FILE_ERROR = 3,        // File system error
    VALIDATION_ERROR = 4   // Data validation failed
}

// Import operation results
enum eUGTriggerImportResult
{
    SUCCESS = 0,           // Import completed successfully
    FILE_NOT_FOUND = 1,    // Import file not found
    PARSE_ERROR = 2,       // JSON parsing failed
    INVALID_DATA = 3,      // Invalid trigger data
    EDITOR_ERROR = 4       // Editor operation failed
}

// Validation utilities for UG triggers
class UGTriggerValidator
{
    // Check if trigger type is valid (0-2)
    static bool IsValidTriggerType(int type)
    {
        return type >= eUGTriggerType.OUTER && type <= eUGTriggerType.TRANSITIONAL;
    }

    // Check eye accommodation range (0.0 to 1.0)
    static bool IsValidEyeAccommodation(float accommodation)
    {
        return accommodation >= 0.0 && accommodation <= 1.0;
    }

    // Check interpolation speed range (0.0 to 1.0)
    static bool IsValidInterpolationSpeed(float speed)
    {
        return speed >= 0.0 && speed <= 1.0;
    }

    // Check trigger size meets minimum dimensions
    static bool IsValidTriggerSize(vector size)
    {
        float minDim = UGTriggerSettings.GetMinDimension();
        return size[0] >= minDim && size[1] >= minDim && size[2] >= minDim;
    }

    // Check breadcrumb radius is positive
    static bool IsValidBreadcrumbRadius(float radius)
    {
        return radius > 0.0;
    }

    // Check position is not zero vector
    static bool IsValidPosition(vector position)
    {
        // Basic position validation - just check it's not a zero vector
        return (position[0] != 0 || position[1] != 0 || position[2] != 0);
    }

    // Get trigger type name
    static string GetTriggerTypeName(int type)
    {
        switch (type)
        {
            case eUGTriggerType.OUTER:
                return "Outer";
            case eUGTriggerType.INNER:
                return "Inner";
            case eUGTriggerType.TRANSITIONAL:
                return "Transitional";
            default:
                return "Unknown";
        }
        return "Unknown";
    }

    // Get default accommodation for trigger type
    static float GetRecommendedAccommodation(int type)
    {
        switch (type)
        {
            case eUGTriggerType.OUTER:
                return UGTriggerSettings.GetDefaultOuterAccommodation();
            case eUGTriggerType.INNER:
            case eUGTriggerType.TRANSITIONAL:
                return UGTriggerSettings.GetDefaultInnerAccommodation();
            default:
                return UGTriggerSettings.GetDefaultOuterAccommodation();
        }
        return UGTriggerSettings.GetDefaultOuterAccommodation();
    }

    // Check if trigger supports breadcrumbs
    static bool SupportsBreadcrumbs(int type)
    {
        return type == eUGTriggerType.TRANSITIONAL;
    }

    // Clamp eye accommodation to valid range
    static float ClampEyeAccommodation(float value)
    {
        return Math.Clamp(value, 0.0, 1.0);
    }

    // Clamp interpolation speed to valid range
    static float ClampInterpolationSpeed(float value)
    {
        return Math.Clamp(value, 0.0, 1.0);
    }

    // Sanitize trigger size to minimum dimensions
    static vector SanitizeTriggerSize(vector size)
    {
        return UGTriggerSettings.ClampSize(size);
    }

    // Validate complete trigger configuration
    static bool IsValidTriggerConfiguration(int type, vector size, float accommodation,
                                          float interpolation, vector position)
    {
        return IsValidTriggerType(type) && IsValidTriggerSize(size) && IsValidEyeAccommodation(accommodation) && IsValidInterpolationSpeed(interpolation) && IsValidPosition(position);
    }
}