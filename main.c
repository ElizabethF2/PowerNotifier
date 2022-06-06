#include <stdio.h>
#include <windows.h>
#include <WinNT.h>
#include <powrprof.h>

#pragma comment(lib, "powrprof")
#pragma comment(lib, "winmm")

#define AC_POWER 0

ULONG callback(_In_opt_ PVOID context, _In_ ULONG type, _In_ PVOID payload)
{
    if ((type == PBT_POWERSETTINGCHANGE) && (payload != NULL))
    {
        PPOWERBROADCAST_SETTING setting = (PPOWERBROADCAST_SETTING)payload;

        if (IsEqualGUID(&GUID_ACDC_POWER_SOURCE, &setting->PowerSetting))
        {
            DWORD value = *(DWORD*)setting->Data;
            if (value == AC_POWER)  // NB: This will treat UPS power and other states as "unplugged"
            {
                printf("Plugged In\n");
                PlaySound(L"plugged_in.wav", NULL, SND_FILENAME);
            }
            else
            {
                printf("Unplugged\n");
                PlaySound(L"unplugged.wav", NULL, SND_FILENAME);
            }
        }
    }
    return ERROR_SUCCESS;
}

//int main()
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS subscription_parameters;
    subscription_parameters.Callback = (PDEVICE_NOTIFY_CALLBACK_ROUTINE)&callback;

    HPOWERNOTIFY registration_handle = NULL;

    // Subscribe for changes in power supply setting
    PowerSettingRegisterNotification(
        &GUID_ACDC_POWER_SOURCE,
        DEVICE_NOTIFY_CALLBACK,
        (HANDLE)&subscription_parameters,
        &registration_handle);

    Sleep(INFINITE);

    return 0;
}
