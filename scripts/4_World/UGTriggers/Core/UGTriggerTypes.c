enum eUGTriggerType
{
    OUTER = 0,         // Outer trigger - high eye accommodation
    INNER = 1,         // Inner trigger - low eye accommodation
    TRANSITIONAL = 2   // Transitional trigger - supports breadcrumbs
}

enum eUGAmbientSoundType
{
    NONE = 0,             
    RAIN = 1,              
    NIGHT = 2,            
    MEADOW = 3,            
    TREES = 4,            
    HILLS = 5,            
    HOUSES = 6,            
    WINDY = 7,            
    DEAD_BODY = 8,         
    SEA = 9,               
    FOREST = 10,           
    ALTITUDE_GROUND = 11,  
    ALTITUDE_SEA = 12,     
    ALTITUDE_SURFACE = 13, 
    DAYTIME = 14,          
    SHOOTING = 15,         
    COAST = 16,           
    WATER_DEPTH = 17,      
    OVERCAST = 18,         
    FOG = 19,              
    SNOWFALL = 20,         
    CAVE_SMALL = 21,       
    CAVE_BIG = 22         
    CONTAMINATION = 23
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

    static bool IsValidInterpolationSpeed(float speed)
    {
        return speed >= UGTriggerSettings.GetMinInterpolation() && speed <= UGTriggerSettings.GetMaxInterpolation();
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

    static float ClampInterpolationSpeed(float value)
    {
        return Math.Clamp(value, UGTriggerSettings.GetMinInterpolation(), UGTriggerSettings.GetMaxInterpolation());
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

    // Convert ambient sound type enum to string for export
    static string GetAmbientSoundTypeString(int soundType)
    {
        switch (soundType)
        {
            case eUGAmbientSoundType.NONE: return "";
            case eUGAmbientSoundType.RAIN: return "rain";
            case eUGAmbientSoundType.NIGHT: return "night";
            case eUGAmbientSoundType.MEADOW: return "meadow";
            case eUGAmbientSoundType.TREES: return "trees";
            case eUGAmbientSoundType.HILLS: return "hills";
            case eUGAmbientSoundType.HOUSES: return "houses";
            case eUGAmbientSoundType.WINDY: return "windy";
            case eUGAmbientSoundType.DEAD_BODY: return "deadBody";
            case eUGAmbientSoundType.SEA: return "sea";
            case eUGAmbientSoundType.FOREST: return "forest";
            case eUGAmbientSoundType.ALTITUDE_GROUND: return "altitudeGround";
            case eUGAmbientSoundType.ALTITUDE_SEA: return "altitudeSea";
            case eUGAmbientSoundType.ALTITUDE_SURFACE: return "altitudeSurface";
            case eUGAmbientSoundType.DAYTIME: return "daytime";
            case eUGAmbientSoundType.SHOOTING: return "shooting";
            case eUGAmbientSoundType.COAST: return "coast";
            case eUGAmbientSoundType.WATER_DEPTH: return "waterDepth";
            case eUGAmbientSoundType.OVERCAST: return "overcast";
            case eUGAmbientSoundType.FOG: return "fog";
            case eUGAmbientSoundType.SNOWFALL: return "snowfall";
            case eUGAmbientSoundType.CAVE_SMALL: return "caveSmall";
            case eUGAmbientSoundType.CAVE_BIG: return "caveBig";
            case eUGAmbientSoundType.CONTAMINATION: return "contamination";
            default: return "";
        }
        return "";
    }

    // Convert ambient sound string to enum for import
    static int GetAmbientSoundTypeFromString(string soundTypeStr)
    {
        if (soundTypeStr == "rain") return eUGAmbientSoundType.RAIN;
        else if (soundTypeStr == "night") return eUGAmbientSoundType.NIGHT;
        else if (soundTypeStr == "meadow") return eUGAmbientSoundType.MEADOW;
        else if (soundTypeStr == "trees") return eUGAmbientSoundType.TREES;
        else if (soundTypeStr == "hills") return eUGAmbientSoundType.HILLS;
        else if (soundTypeStr == "houses") return eUGAmbientSoundType.HOUSES;
        else if (soundTypeStr == "windy") return eUGAmbientSoundType.WINDY;
        else if (soundTypeStr == "deadBody") return eUGAmbientSoundType.DEAD_BODY;
        else if (soundTypeStr == "sea") return eUGAmbientSoundType.SEA;
        else if (soundTypeStr == "forest") return eUGAmbientSoundType.FOREST;
        else if (soundTypeStr == "altitudeGround") return eUGAmbientSoundType.ALTITUDE_GROUND;
        else if (soundTypeStr == "altitudeSea") return eUGAmbientSoundType.ALTITUDE_SEA;
        else if (soundTypeStr == "altitudeSurface") return eUGAmbientSoundType.ALTITUDE_SURFACE;
        else if (soundTypeStr == "daytime") return eUGAmbientSoundType.DAYTIME;
        else if (soundTypeStr == "shooting") return eUGAmbientSoundType.SHOOTING;
        else if (soundTypeStr == "coast") return eUGAmbientSoundType.COAST;
        else if (soundTypeStr == "waterDepth") return eUGAmbientSoundType.WATER_DEPTH;
        else if (soundTypeStr == "overcast") return eUGAmbientSoundType.OVERCAST;
        else if (soundTypeStr == "fog") return eUGAmbientSoundType.FOG;
        else if (soundTypeStr == "snowfall") return eUGAmbientSoundType.SNOWFALL;
        else if (soundTypeStr == "caveSmall") return eUGAmbientSoundType.CAVE_SMALL;
        else if (soundTypeStr == "caveBig") return eUGAmbientSoundType.CAVE_BIG;
        else if (soundTypeStr == "contamination") return eUGAmbientSoundType.CONTAMINATION;
        else return eUGAmbientSoundType.NONE;
        return eUGAmbientSoundType.NONE;
    }

    // Validate ambient sound type string
    static bool IsValidAmbientSoundType(string soundTypeStr)
    {
        if (soundTypeStr == "") return true;
        if (soundTypeStr == "rain") return true;
        if (soundTypeStr == "night") return true;
        if (soundTypeStr == "meadow") return true;
        if (soundTypeStr == "trees") return true;
        if (soundTypeStr == "hills") return true;
        if (soundTypeStr == "houses") return true;
        if (soundTypeStr == "windy") return true;
        if (soundTypeStr == "deadBody") return true;
        if (soundTypeStr == "sea") return true;
        if (soundTypeStr == "forest") return true;
        if (soundTypeStr == "altitudeGround") return true;
        if (soundTypeStr == "altitudeSea") return true;
        if (soundTypeStr == "altitudeSurface") return true;
        if (soundTypeStr == "daytime") return true;
        if (soundTypeStr == "shooting") return true;
        if (soundTypeStr == "coast") return true;
        if (soundTypeStr == "waterDepth") return true;
        if (soundTypeStr == "overcast") return true;
        if (soundTypeStr == "fog") return true;
        if (soundTypeStr == "snowfall") return true;
        if (soundTypeStr == "caveSmall") return true;
        if (soundTypeStr == "caveBig") return true;
        if (soundTypeStr == "contamination") return true;
        return false;
    }

    // Get available ambient sound type options for UI dropdowns
    static void GetAmbientSoundTypeOptions(out array<string> options, out array<int> values)
    {
        if (!options) options = new array<string>();
        if (!values) values = new array<int>();

        options.Clear();
        values.Clear();

        options.Insert("None");
        values.Insert(eUGAmbientSoundType.NONE);

        options.Insert("Rain");
        values.Insert(eUGAmbientSoundType.RAIN);

        options.Insert("Night");
        values.Insert(eUGAmbientSoundType.NIGHT);

        options.Insert("Meadow");
        values.Insert(eUGAmbientSoundType.MEADOW);

        options.Insert("Trees");
        values.Insert(eUGAmbientSoundType.TREES);

        options.Insert("Hills");
        values.Insert(eUGAmbientSoundType.HILLS);

        options.Insert("Houses");
        values.Insert(eUGAmbientSoundType.HOUSES);

        options.Insert("Windy");
        values.Insert(eUGAmbientSoundType.WINDY);

        options.Insert("Dead Body");
        values.Insert(eUGAmbientSoundType.DEAD_BODY);

        options.Insert("Sea");
        values.Insert(eUGAmbientSoundType.SEA);

        options.Insert("Forest");
        values.Insert(eUGAmbientSoundType.FOREST);

        options.Insert("Altitude Ground");
        values.Insert(eUGAmbientSoundType.ALTITUDE_GROUND);

        options.Insert("Altitude Sea");
        values.Insert(eUGAmbientSoundType.ALTITUDE_SEA);

        options.Insert("Altitude Surface");
        values.Insert(eUGAmbientSoundType.ALTITUDE_SURFACE);

        options.Insert("Daytime");
        values.Insert(eUGAmbientSoundType.DAYTIME);

        options.Insert("Shooting");
        values.Insert(eUGAmbientSoundType.SHOOTING);

        options.Insert("Coast");
        values.Insert(eUGAmbientSoundType.COAST);

        options.Insert("Water Depth");
        values.Insert(eUGAmbientSoundType.WATER_DEPTH);

        options.Insert("Overcast");
        values.Insert(eUGAmbientSoundType.OVERCAST);

        options.Insert("Fog");
        values.Insert(eUGAmbientSoundType.FOG);

        options.Insert("Snowfall");
        values.Insert(eUGAmbientSoundType.SNOWFALL);

        options.Insert("Cave Small");
        values.Insert(eUGAmbientSoundType.CAVE_SMALL);

        options.Insert("Cave Big");
        values.Insert(eUGAmbientSoundType.CAVE_BIG);

        options.Insert("Contamination");
        values.Insert(eUGAmbientSoundType.CONTAMINATION);
    }

    // Get display name for ambient sound type
    static string GetAmbientSoundTypeDisplayName(int soundType)
    {
        switch (soundType)
        {
            case eUGAmbientSoundType.NONE: return "None";
            case eUGAmbientSoundType.RAIN: return "Rain";
            case eUGAmbientSoundType.NIGHT: return "Night";
            case eUGAmbientSoundType.MEADOW: return "Meadow";
            case eUGAmbientSoundType.TREES: return "Trees";
            case eUGAmbientSoundType.HILLS: return "Hills";
            case eUGAmbientSoundType.HOUSES: return "Houses";
            case eUGAmbientSoundType.WINDY: return "Windy";
            case eUGAmbientSoundType.DEAD_BODY: return "Dead Body";
            case eUGAmbientSoundType.SEA: return "Sea";
            case eUGAmbientSoundType.FOREST: return "Forest";
            case eUGAmbientSoundType.ALTITUDE_GROUND: return "Altitude Ground";
            case eUGAmbientSoundType.ALTITUDE_SEA: return "Altitude Sea";
            case eUGAmbientSoundType.ALTITUDE_SURFACE: return "Altitude Surface";
            case eUGAmbientSoundType.DAYTIME: return "Daytime";
            case eUGAmbientSoundType.SHOOTING: return "Shooting";
            case eUGAmbientSoundType.COAST: return "Coast";
            case eUGAmbientSoundType.WATER_DEPTH: return "Water Depth";
            case eUGAmbientSoundType.OVERCAST: return "Overcast";
            case eUGAmbientSoundType.FOG: return "Fog";
            case eUGAmbientSoundType.SNOWFALL: return "Snowfall";
            case eUGAmbientSoundType.CAVE_SMALL: return "Cave Small";
            case eUGAmbientSoundType.CAVE_BIG: return "Cave Big";
            case eUGAmbientSoundType.CONTAMINATION: return "Contamination";
            default: return "Unknown";
        }
        return "Unknown";
    }
}