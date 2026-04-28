// Centralized error handling and logging for UG Triggers
class UGTriggerErrorHandler
{
    // Error severity levels
    protected static const int SEVERITY_INFO = 0;
    protected static const int SEVERITY_WARNING = 1;
    protected static const int SEVERITY_ERROR = 2;
    protected static const int SEVERITY_CRITICAL = 3;

    // Error categories
    protected static const string CATEGORY_EXPORT = "Export";
    protected static const string CATEGORY_IMPORT = "Import";
    protected static const string CATEGORY_VALIDATION = "Validation";
    protected static const string CATEGORY_EDITOR = "Editor";
    protected static const string CATEGORY_SYSTEM = "System";

    // Log info message
    static void LogInfo(string category, string message)
    {
        LogMessage(SEVERITY_INFO, category, message);
    }

    // Log warning message
    static void LogWarning(string category, string message)
    {
        LogMessage(SEVERITY_WARNING, category, message);
    }

    // Log error message
    static void LogError(string category, string message)
    {
        LogMessage(SEVERITY_ERROR, category, message);
    }

    // Log critical error message
    static void LogCritical(string category, string message)
    {
        LogMessage(SEVERITY_CRITICAL, category, message);
    }

    protected static void LogMessage(int severity, string category, string message)
    {
        string severityText = GetSeverityText(severity);
        string timestamp = GetFormattedTime();
        string fullMessage = string.Format("[UG Triggers] %1 [%2] %3: %4",
                                          timestamp, severityText, category, message);

        if (severity >= SEVERITY_WARNING)
            Print(fullMessage);

        if (severity >= SEVERITY_ERROR)
        {
            ShowUserNotification(severityText, category, message);
        }
    }

    protected static string GetSeverityText(int severity)
    {
        switch (severity)
        {
            case SEVERITY_INFO: return "INFO";
            case SEVERITY_WARNING: return "WARN";
            case SEVERITY_ERROR: return "ERROR";
            case SEVERITY_CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    protected static string GetFormattedTime()
    {
        float time = UGEditorGameCache.GetCachedTime();
        int totalSeconds = time;
        int hours = (totalSeconds / 3600);
        int minutes = (totalSeconds - hours * 3600) / 60;
        int seconds = totalSeconds - hours * 3600 - minutes * 60;
        return string.Format("%1:%2:%3", hours.ToString(), minutes.ToString(), seconds.ToString());
    }

    // Show error notification to user
    protected static void ShowUserNotification(string severityText, string category, string message)
    {
        Editor editor = UGEditorGameCache.GetCachedEditor();
        if (editor && editor.GetEditorHud())
        {
            string notification = string.Format("[%1] %2: %3", severityText, category, message);
            editor.GetEditorHud().CreateNotification(notification);
        }
    }

    // Specialized error handling methods for common scenarios

    static void HandleExportError(eUGTriggerExportResult result, string details = "")
    {
        string message = GetExportErrorMessage(result);
        if (details != "")
        {
            message = message + " - " + details;
        }

        int severity = GetExportErrorSeverity(result);
        LogMessage(severity, CATEGORY_EXPORT, message);
    }

    static void HandleImportError(eUGTriggerImportResult result, string details = "")
    {
        string message = GetImportErrorMessage(result);
        if (details != "")
        {
            message = message + " - " + details;
        }

        int severity = GetImportErrorSeverity(result);
        LogMessage(severity, CATEGORY_IMPORT, message);
    }

    static void HandleValidationError(string parameter, string value, string expectedRange)
    {
        string message = string.Format("Invalid %1 value '%2', expected %3", parameter, value, expectedRange);
        LogError(CATEGORY_VALIDATION, message);
    }

    static void HandleEditorError(string operation, string reason)
    {
        string message = string.Format("%1 failed: %2", operation, reason);
        LogError(CATEGORY_EDITOR, message);
    }

    static void HandleSystemError(string component, string reason)
    {
        string message = string.Format("%1 initialization failed: %2", component, reason);
        LogCritical(CATEGORY_SYSTEM, message);
    }

    protected static string GetExportErrorMessage(eUGTriggerExportResult result)
    {
        switch (result)
        {
            case eUGTriggerExportResult.SUCCESS:
                return "Export completed successfully";
            case eUGTriggerExportResult.EDITOR_NOT_FOUND:
                return "Editor instance not available";
            case eUGTriggerExportResult.NO_OBJECTS:
                return "No trigger objects found to export";
            case eUGTriggerExportResult.FILE_ERROR:
                return "File system error during export";
            case eUGTriggerExportResult.VALIDATION_ERROR:
                return "Data validation failed during export";
            default:
                return "Unknown export error";
        }
        return "Unknown export error";
    }

    protected static string GetImportErrorMessage(eUGTriggerImportResult result)
    {
        switch (result)
        {
            case eUGTriggerImportResult.SUCCESS:
                return "Import completed successfully";
            case eUGTriggerImportResult.FILE_NOT_FOUND:
                return "Import file not found";
            case eUGTriggerImportResult.PARSE_ERROR:
                return "Failed to parse import file";
            case eUGTriggerImportResult.INVALID_DATA:
                return "Invalid trigger data in import file";
            case eUGTriggerImportResult.EDITOR_ERROR:
                return "Editor operation failed during import";
            default:
                return "Unknown import error";
        }
        return "Unknown import error";
    }

    protected static int GetExportErrorSeverity(eUGTriggerExportResult result)
    {
        switch (result)
        {
            case eUGTriggerExportResult.SUCCESS:
                return SEVERITY_INFO;
            case eUGTriggerExportResult.NO_OBJECTS:
                return SEVERITY_WARNING;
            case eUGTriggerExportResult.EDITOR_NOT_FOUND:
            case eUGTriggerExportResult.FILE_ERROR:
            case eUGTriggerExportResult.VALIDATION_ERROR:
                return SEVERITY_ERROR;
            default:
                return SEVERITY_ERROR;
        }
        return SEVERITY_ERROR;
    }

    protected static int GetImportErrorSeverity(eUGTriggerImportResult result)
    {
        switch (result)
        {
            case eUGTriggerImportResult.SUCCESS:
                return SEVERITY_INFO;
            case eUGTriggerImportResult.FILE_NOT_FOUND:
            case eUGTriggerImportResult.PARSE_ERROR:
            case eUGTriggerImportResult.INVALID_DATA:
            case eUGTriggerImportResult.EDITOR_ERROR:
                return SEVERITY_ERROR;
            default:
                return SEVERITY_ERROR;
        }
        return SEVERITY_ERROR;
    }

    static bool ValidateAndReport(int type, vector size, float accommodation,
                                float interpolation, vector position)
    {
        bool isValid = true;

        if (!UGTriggerValidator.IsValidTriggerType(type))
        {
            HandleValidationError("trigger type", type.ToString(), "0-2");
            isValid = false;
        }

        if (!UGTriggerValidator.IsValidTriggerSize(size))
        {
            string sizeStr = string.Format("(%1, %2, %3)", size[0], size[1], size[2]);
            string minStr = string.Format(">= %1 for all axes", UGTriggerSettings.GetMinDimension());
            HandleValidationError("trigger size", sizeStr, minStr);
            isValid = false;
        }

        if (!UGTriggerValidator.IsValidEyeAccommodation(accommodation))
        {
            HandleValidationError("eye accommodation", accommodation.ToString(), "0.0-1.0");
            isValid = false;
        }

        if (!UGTriggerValidator.IsValidInterpolationSpeed(interpolation))
        {
            HandleValidationError("interpolation speed", interpolation.ToString(), string.Format("%1 to %2", UGTriggerSettings.GetMinInterpolation(), UGTriggerSettings.GetMaxInterpolation()));
            isValid = false;
        }

        if (!UGTriggerValidator.IsValidPosition(position))
        {
            string posStr = string.Format("(%1, %2, %3)", position[0], position[1], position[2]);
            HandleValidationError("position", posStr, "finite values");
            isValid = false;
        }

        return isValid;
    }
}