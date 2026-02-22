/*
 * CamEditor - Modular Include
 * Mutlak 100% buatan w (Engkq)
 * Updated to open.mp - thanks all
 */

#if defined _cam_editor_included
    #endinput
#endif
#define _cam_editor_included

#include <open.mp>
#include <YSI_Coding\y_hooks>

//==========================================================================
// DEFINITIONS
//==========================================================================

#define MOVE_SPEED              100.0
#define ACCEL_RATE              0.03

#define CAMERA_MODE_NONE        0
#define CAMERA_MODE_FLY         1
 
#define MOVE_FWD                1
#define MOVE_BACK               2
#define MOVE_LEFT               3
#define MOVE_RIGHT              4
#define MOVE_FWD_L              5
#define MOVE_FWD_R              6
#define MOVE_BACK_L             7
#define MOVE_BACK_R             8

#define DIALOG_MENU             8878
#define DIALOG_MOVE_SPEED       8879
#define DIALOG_ROT_SPEED        8880
#define DIALOG_EXPORTNAME       8881
#define DIALOG_CLOSE_NEW        8882

//==========================================================================
// VARIABLES & ENUMS
//==========================================================================

static const Float:fScale = 5.0;

static 
    MenuTimer,
    Float:fPX, Float:fPY, Float:fPZ,
    Float:fVX, Float:fVY, Float:fVZ,
    Float:object_x, Float:object_y, Float:object_z;

static 
    bool:IsCreating[MAX_PLAYERS],
    bool:IsReSettingStart[MAX_PLAYERS],
    bool:IsReSettingEnd[MAX_PLAYERS],
    bool:SettingFirstLoc[MAX_PLAYERS],
    bool:SettingLastLoc[MAX_PLAYERS],
    bool:IsCamMoving[MAX_PLAYERS];
 
enum noclipenum
{
    cameramode,
    flyobject,
    mode,
    lrold,
    udold,
    lastmove,
    Float:accelmul
}

static noclipdata[MAX_PLAYERS][noclipenum];
 
enum Coordinates
{
    Float:StartX, Float:StartY, Float:StartZ,
    Float:EndX, Float:EndY, Float:EndZ,
    Float:StartLookX, Float:StartLookY, Float:StartLookZ,
    Float:EndLookX, Float:EndLookY, Float:EndLookZ,
    MoveSpeed, RotSpeed
}

static coordInfo[MAX_PLAYERS][Coordinates];

forward bool:IsStringNumeric(const szInput[]);

//==========================================================================
// HOOKS
//==========================================================================

hook OnPlayerDisconnect(playerid, reason)
{
    if (noclipdata[playerid][cameramode] == CAMERA_MODE_FLY) 
    {
        CancelFlyMode(playerid);
    }
    return 1;
}

//==========================================================================
// HOOKS - CONTINUED
//==========================================================================

hook OnPlayerConnect(playerid)
{
    noclipdata[playerid][cameramode]    = CAMERA_MODE_NONE;
    noclipdata[playerid][lrold]         = 0;
    noclipdata[playerid][udold]         = 0;
    noclipdata[playerid][mode]          = 0;
    noclipdata[playerid][lastmove]      = 0;
    noclipdata[playerid][accelmul]      = 0.0;
    IsCreating[playerid]                = false;
    IsReSettingStart[playerid]          = false;
    IsReSettingEnd[playerid]            = false;
    SettingFirstLoc[playerid]           = false;
    SettingLastLoc[playerid]            = false;
    IsCamMoving[playerid]               = false;
    coordInfo[playerid][MoveSpeed]      = 1000;
    coordInfo[playerid][RotSpeed]       = 1000;
    return 1;
}

hook OnPlayerSpawn(playerid)
{
    if (IsCreating[playerid] == false)
    {
        SendClientMessage(playerid, -1, "{FFFFFF}Type {8EFF8E}/cameditor{FFFFFF} to access the camera editor");
    }
    return 1;
}

// Menggunakan hook OnPlayerKeyStateChange
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if ((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE))
    {
        if (IsCreating[playerid] == true)
        {
            if ((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE))
            {
                // Pastikan pemain sedang menggunakan editor
                if (IsCreating[playerid])
                {
                    // Panggil fungsi utama untuk memproses titik kamera
                    ProcessCameraPoint(playerid);
                    return 1;
                }
            }
            if (SettingFirstLoc[playerid] == true)
            {
                GetPlayerCameraPos(playerid, fPX, fPY, fPZ);
                GetPlayerCameraFrontVector(playerid, fVX, fVY, fVZ);
                object_x = fPX + floatmul(fVX, fScale);
                object_y = fPY + floatmul(fVY, fScale);
                object_z = fPZ + floatmul(fVZ, fScale);
                
                coordInfo[playerid][StartX]         = fPX;
                coordInfo[playerid][StartY]         = fPY;
                coordInfo[playerid][StartZ]         = fPZ;
                coordInfo[playerid][StartLookX]     = object_x;
                coordInfo[playerid][StartLookY]     = object_y;
                coordInfo[playerid][StartLookZ]     = object_z;

                if (IsReSettingStart[playerid] == true)
                {
                    SendClientMessage(playerid, -1, "{8EFF8E}>{FFFFFF} Start point has been {8EFF8E}successfully re-set");
                    ShowPlayerDialog(playerid, DIALOG_MENU, DIALOG_STYLE_LIST,"{FFFFFF}Camera Editor - Next Step?","{FFFFFF}Preview Camera Movement\nModify Start Point\nModify End Point\nAdjust Speed Settings\nSave to File","{8EFF8E}Confirm","{F58282}Cancel");
                    
                    IsReSettingStart[playerid]      = false;
                    IsReSettingEnd[playerid]        = false;
                    SettingFirstLoc[playerid]       = false;
                    SettingLastLoc[playerid]        = false;
                }
                else
                {
                    SendClientMessage(playerid, -1, "{8EFF8E}Start point{FFFFFF} saved successfully");
                    SendClientMessage(playerid, -1, "{FFFFFF}Now press {F58282}~k~~PED_FIREWEAPON~{FFFFFF} to define the {8EFF8E}end point{FFFFFF}");
                    SettingLastLoc[playerid] = true;
                    SettingFirstLoc[playerid] = false;
                }
            }
            else if (SettingLastLoc[playerid] == true)
            {
                new string[512];
                format(string, sizeof(string), "{FFFFFF}Please enter the desired {F58282}movement{FFFFFF} duration in milliseconds\n\nCurrent movement speed: \t{F58282}%i milliseconds\n{FFFFFF}Current rotation speed: \t{F58282}%i milliseconds\n\n\n{CFCFCF}Note: {FFFFFF}1 second = 1000 milliseconds", coordInfo[playerid][MoveSpeed], coordInfo[playerid][RotSpeed]);
                ShowPlayerDialog(playerid, DIALOG_MOVE_SPEED, DIALOG_STYLE_INPUT, "{FFFFFF}Movement Duration Settings", string,"{8EFF8E}Confirm","{F58282}Cancel");
                
                GetPlayerCameraPos(playerid, fPX, fPY, fPZ);
                GetPlayerCameraFrontVector(playerid, fVX, fVY, fVZ);

                object_x = fPX + floatmul(fVX, fScale);
                object_y = fPY + floatmul(fVY, fScale);
                object_z = fPZ + floatmul(fVZ, fScale);

                coordInfo[playerid][EndX]           = fPX;
                coordInfo[playerid][EndY]           = fPY;
                coordInfo[playerid][EndZ]           = fPZ;
                coordInfo[playerid][EndLookX]       = object_x;
                coordInfo[playerid][EndLookY]       = object_y;
                coordInfo[playerid][EndLookZ]       = object_z;

                if (IsReSettingEnd[playerid] == true)
                {
                    SendClientMessage(playerid, -1, "{8EFF8E}>{FFFFFF} end point has been {8EFF8E}successfully re-set");
                    ShowPlayerDialog(playerid, DIALOG_MENU, DIALOG_STYLE_LIST,"{FFFFFF}Camera Editor - Next Step?","{FFFFFF}Preview\nChange Start\nChange End\nChange Speed\nSave","{8EFF8E}Confirm","{F58282}Cancel");

                    IsReSettingStart[playerid]      = false;
                    IsReSettingEnd[playerid]        = false;
                    SettingFirstLoc[playerid]       = false;
                    SettingLastLoc[playerid]        = false;
                }
                else
                {
                    SendClientMessage(playerid, -1, "{8EFF8E}End point{FFFFFF} saved successfully");
                    SettingLastLoc[playerid] = false;
                }
            }
        }
    }
    return 1;
}

//==========================================================================
// COMMANDS
//==========================================================================

CMD:setpoint(playerid, params[])
{
    if (!IsCreating[playerid]) 
        return SendClientMessage(playerid, -1, "{FF0000}Error: {FFFFFF}Editor kamera tidak aktif!");

    ProcessCameraPoint(playerid);
    return 1;
}

CMD:cameditor(playerid, params[])
{

    if (IsCamMoving[playerid]) 
        return SendClientMessage(playerid, -1, "{F58282}Error: {FFFFFF}Tunggu hingga pergerakan kamera selesai.");

    if (GetPVarType(playerid, "FlyMode"))
    {
        CancelFlyMode(playerid);
        IsCreating[playerid] = false;
    }
    else 
    {
        FlyMode(playerid);
    }
    return 1;
}

CMD:closecameditor(playerid, params[])
{

    if (!IsCreating[playerid])
    {
        SendClientMessage(playerid, -1, "{F58282}Error: {FFFFFF}Camera Editor tidak sedang aktif.");
        return SendClientMessage(playerid, -1, "{FFFFFF}Gunakan {8EFF8E}/cameditor{FFFFFF} untuk memulai.");
    }

    CancelFlyMode(playerid);
    
    // Reset Data
    IsCreating[playerid]                = false;
    noclipdata[playerid][cameramode]    = CAMERA_MODE_NONE;
    noclipdata[playerid][lrold]         = 0;
    noclipdata[playerid][udold]         = 0;
    noclipdata[playerid][mode]          = 0;
    noclipdata[playerid][lastmove]      = 0;
    noclipdata[playerid][accelmul]      = 0.0;
    IsReSettingStart[playerid]          = false;
    IsReSettingEnd[playerid]            = false;
    SettingFirstLoc[playerid]           = false;
    SettingLastLoc[playerid]            = false;
    IsCamMoving[playerid]               = false;
    coordInfo[playerid][MoveSpeed]      = 1000;
    coordInfo[playerid][RotSpeed]       = 1000;
    
    SendClientMessage(playerid, -1, "{8EFF8E}Camera Editor{FFFFFF} telah ditutup.");
    return 1;
}

//==========================================================================
// HOOKS: UPDATE & DIALOG
//==========================================================================

hook OnPlayerUpdate(playerid)
{
    if (noclipdata[playerid][cameramode] == CAMERA_MODE_FLY)
    {
        new KEY:keys, updown, leftright;
        GetPlayerKeys(playerid, keys, updown, leftright);
 
        // Logika pergerakan kamera berdasarkan input tombol
        if (noclipdata[playerid][mode] && (GetTickCount() - noclipdata[playerid][lastmove] > 100))
        {
            MoveCamera(playerid);
        }

        if (noclipdata[playerid][udold] != updown || noclipdata[playerid][lrold] != leftright)
        {
            // Jika tombol dilepas (berhenti bergerak)
            if ((noclipdata[playerid][udold] != 0 || noclipdata[playerid][lrold] != 0) && updown == 0 && leftright == 0)
            {
                StopPlayerObject(playerid, noclipdata[playerid][flyobject]);
                noclipdata[playerid][mode]      = 0;
                noclipdata[playerid][accelmul]  = 0.0;
            }
            else // Jika ada tombol arah yang ditekan
            {
                noclipdata[playerid][mode] = GetMoveDirectionFromKeys(updown, leftright);
                MoveCamera(playerid);
            }
        }

        noclipdata[playerid][udold] = updown; 
        noclipdata[playerid][lrold] = leftright;

        return 0; // Mengurangi beban sinkronisasi saat sedang mengedit
    }
    return 1;
}

hook OnDialogResponse@CamEditor(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
    case DIALOG_MENU:
        {
            if (!response) 
            {
                CancelFlyMode(playerid);
                IsCreating[playerid] = false;
                return SendClientMessage(playerid, -1, "{F58282}Editor:{FFFFFF} Kamu telah keluar dari editor.");
            }

            switch(listitem)
            {
                case 0: PreviewMovement(playerid);
                case 1: 
                { // Re-set Start
                    IsReSettingStart[playerid] = true;
                    SettingFirstLoc[playerid] = true;
                    SettingLastLoc[playerid] = false;
                    SendClientMessage(playerid, -1, "{FFFF00}Cari posisi {8EFF8E}START{FFFF00} baru dan tekan FIRE.");
                }
                case 2: 
                { // Re-set End
                    IsReSettingEnd[playerid] = true;
                    SettingLastLoc[playerid] = true;
                    SettingFirstLoc[playerid] = false;
                    SendClientMessage(playerid, -1, "{FFFF00}Cari posisi {F58282}END{FFFF00} baru dan tekan FIRE.");
                }
                case 3: ShowSpeedDialog(playerid);
                case 4: ShowPlayerDialog(playerid, DIALOG_EXPORTNAME, DIALOG_STYLE_INPUT, "{FFFFFF}Save Movement", "Masukkan nama untuk file ini:", "Save", "Back");
            }
            return 1;
        }

        case DIALOG_MOVE_SPEED, DIALOG_ROT_SPEED:
        {
            if (!response) return ShowPlayerMenu(playerid);
            
            if (!IsStringNumeric(inputtext)) 
            {
                SendClientMessage(playerid, -1, "{F58282}Error:{FFFFFF} Masukkan angka saja!");
                return ShowSpeedDialog(playerid);
            }

            if (dialogid == DIALOG_MOVE_SPEED) 
            {
                coordInfo[playerid][MoveSpeed] = strval(inputtext);
                ShowPlayerDialog(playerid, DIALOG_ROT_SPEED, DIALOG_STYLE_INPUT, "{FFFFFF}Rotation Settings", "Masukkan durasi rotasi (ms):", "Confirm", "Back");
            } 
            else 
            {
                coordInfo[playerid][RotSpeed] = strval(inputtext);
                ShowPlayerMenu(playerid);
            }
            return 1;
        }

        case DIALOG_EXPORTNAME:
        {
            if (!response) return ShowPlayerMenu(playerid);
            if (!strlen(inputtext)) return ShowPlayerDialog(playerid, DIALOG_EXPORTNAME, DIALOG_STYLE_INPUT, "{FFFFFF}Save", "Nama tidak boleh kosong!", "Save", "Back");
            
            ExportMovement(playerid, inputtext);
            return 1;
        }

        case DIALOG_CLOSE_NEW:
        {
            if (response) {
                IsCreating[playerid] = true;
                SettingFirstLoc[playerid] = true;
                FlyMode(playerid);
            } else {
                CancelFlyMode(playerid);
                IsCreating[playerid] = false;
                SendClientMessage(playerid, -1, "{8EFF8E}Editor:{FFFFFF} Selesai.");
            }
            return 1;
        }
    }
    return 1;
}

//==========================================================================
// PUBLIC FUNCTIONS & FORWARDS
//==========================================================================

forward ShowPlayerMenu(playerid);
public ShowPlayerMenu(playerid)
{
    KillTimer(MenuTimer);
    IsCamMoving[playerid] = false;

    ShowPlayerDialog(playerid, DIALOG_MENU, DIALOG_STYLE_LIST, 
        "{FFFFFF}Camera Editor - Next Step?", 
        "{FFFFFF}Preview\nChange Start\nChange End\nChange Speed\nSave", 
        "{8EFF8E}Confirm", "{F58282}Cancel");

    return 1;
}

forward PreviewMovement(playerid);
public PreviewMovement(playerid)
{
    IsCamMoving[playerid] = true;

    // Gunakan DestroyPlayerObject karena flyobject dibuat per-player
    DestroyPlayerObject(playerid, noclipdata[playerid][flyobject]);
    SetCameraBehindPlayer(playerid);

    // Ambil durasi terlama antara Move dan Rotasi untuk timer menu
    new duration = (coordInfo[playerid][MoveSpeed] > coordInfo[playerid][RotSpeed]) ? 
                    coordInfo[playerid][MoveSpeed] : coordInfo[playerid][RotSpeed];

    // Munculkan menu kembali setelah pergerakan selesai (+ sedikit delay)
    MenuTimer = SetTimerEx("ShowPlayerMenu", duration + 100, false, "i", playerid);

    // Jalankan pergerakan kamera
    InterpolateCameraPos(playerid, 
        coordInfo[playerid][StartX], coordInfo[playerid][StartY], coordInfo[playerid][StartZ], 
        coordInfo[playerid][EndX], coordInfo[playerid][EndY], coordInfo[playerid][EndZ], 
        coordInfo[playerid][MoveSpeed]);

    InterpolateCameraLookAt(playerid, 
        coordInfo[playerid][StartLookX], coordInfo[playerid][StartLookY], coordInfo[playerid][StartLookZ], 
        coordInfo[playerid][EndLookX], coordInfo[playerid][EndLookY], coordInfo[playerid][EndLookZ], 
        coordInfo[playerid][RotSpeed]);

    return 1;
}

forward ExportMovement(playerid, inputtext[]);
public ExportMovement(playerid, inputtext[])
{
    new filename[64], string[512];
    format(filename, sizeof(filename), "CamEdit_%s.txt", inputtext);
    
    new File:f = fopen(filename, io_write);
    if(f)
    {
        format(string, sizeof(string), "|---------- %s ----------|\r\n", inputtext);
        fwrite(f, string);

        format(string, sizeof(string), "InterpolateCameraPos(playerid, %f, %f, %f, %f, %f, %f, %i);\r\n",
            coordInfo[playerid][StartX], coordInfo[playerid][StartY], coordInfo[playerid][StartZ], 
            coordInfo[playerid][EndX], coordInfo[playerid][EndY], coordInfo[playerid][EndZ], 
            coordInfo[playerid][MoveSpeed]);
        fwrite(f, string);

        format(string, sizeof(string), "InterpolateCameraLookAt(playerid, %f, %f, %f, %f, %f, %f, %i);\r\n",
            coordInfo[playerid][StartLookX], coordInfo[playerid][StartLookY], coordInfo[playerid][StartLookZ], 
            coordInfo[playerid][EndLookX], coordInfo[playerid][EndLookY], coordInfo[playerid][EndLookZ], 
            coordInfo[playerid][RotSpeed]);
        fwrite(f, string);

        fclose(f);

        format(string, sizeof(string), "{FFFFFF}Gerakan kamera disimpan sebagai {F58282}%s{FFFFFF}\ndi folder scriptfiles!\n\nApa yang ingin kamu lakukan selanjutnya?", filename);
        ShowPlayerDialog(playerid, DIALOG_CLOSE_NEW, DIALOG_STYLE_MSGBOX, "{FFFFFF}Success", string, "{8EFF8E}Create New", "{F58282}Exit");
    }
    return 1;
}

//==========================================================================
// CORE FUNCTIONS
//==========================================================================

stock ProcessCameraPoint(playerid)
{
    GetPlayerCameraPos(playerid, fPX, fPY, fPZ);
    GetPlayerCameraFrontVector(playerid, fVX, fVY, fVZ);
    
    object_x = fPX + (fVX * 5.0);
    object_y = fPY + (fVY * 5.0);
    object_z = fPZ + (fVZ * 5.0);

    if (SettingFirstLoc[playerid])
    {
        coordInfo[playerid][StartX] = fPX;
        coordInfo[playerid][StartY] = fPY;
        coordInfo[playerid][StartZ] = fPZ;
        coordInfo[playerid][StartLookX] = object_x;
        coordInfo[playerid][StartLookY] = object_y;
        coordInfo[playerid][StartLookZ] = object_z;

        if (IsReSettingStart[playerid])
        {
            SendClientMessage(playerid, -1, "{8EFF8E}>{FFFFFF} Start point re-set!");
            ShowPlayerMenu(playerid);
            IsReSettingStart[playerid] = false;
        }
        else
        {
            SendClientMessage(playerid, -1, "{8EFF8E}Start point saved.");
            SettingLastLoc[playerid] = true;
            SettingFirstLoc[playerid] = false;
        }
    }
    else if (SettingLastLoc[playerid])
    {
        coordInfo[playerid][EndX] = fPX;
        coordInfo[playerid][EndY] = fPY;
        coordInfo[playerid][EndZ] = fPZ;
        coordInfo[playerid][EndLookX] = object_x;
        coordInfo[playerid][EndLookY] = object_y;
        coordInfo[playerid][EndLookZ] = object_z;

        if (IsReSettingEnd[playerid])
        {
            SendClientMessage(playerid, -1, "{8EFF8E}>{FFFFFF} End point re-set!");
            ShowPlayerMenu(playerid);
            IsReSettingEnd[playerid] = false;
        }
        else
        {
            SendClientMessage(playerid, -1, "{8EFF8E}End point saved.");
            SettingLastLoc[playerid] = false;
            ShowSpeedDialog(playerid); 
        }
    }
    return 1;
}

stock MoveCamera(playerid)
{
    new Float:FV[3], Float:CP[3];

    GetPlayerCameraPos(playerid, CP[0], CP[1], CP[2]);
    GetPlayerCameraFrontVector(playerid, FV[0], FV[1], FV[2]);

    // Akselerasi pergerakan
    if (noclipdata[playerid][accelmul] <= 1.0) 
    {
        noclipdata[playerid][accelmul] += ACCEL_RATE;
    }

    new Float:speed = MOVE_SPEED * noclipdata[playerid][accelmul],
        Float:X, Float:Y, Float:Z;

    GetNextCameraPosition(noclipdata[playerid][mode], CP, FV, X, Y, Z);
    MovePlayerObject(playerid, noclipdata[playerid][flyobject], X, Y, Z, speed);
    
    noclipdata[playerid][lastmove] = GetTickCount();
    return 1;
}

stock GetNextCameraPosition(move_mode, const Float:CP[3], const Float:FV[3], &Float:X, &Float:Y, &Float:Z)
{
    #define OFFSET_X (FV[0] * 6000.0)
    #define OFFSET_Y (FV[1] * 6000.0)
    #define OFFSET_Z (FV[2] * 6000.0)

    switch(move_mode)
    {
        case MOVE_FWD:      { X = CP[0]+OFFSET_X; Y = CP[1]+OFFSET_Y; Z = CP[2]+OFFSET_Z; }
        case MOVE_BACK:     { X = CP[0]-OFFSET_X; Y = CP[1]-OFFSET_Y; Z = CP[2]-OFFSET_Z; }
        case MOVE_LEFT:     { X = CP[0]-OFFSET_Y; Y = CP[1]+OFFSET_X; Z = CP[2]; }
        case MOVE_RIGHT:    { X = CP[0]+OFFSET_Y; Y = CP[1]-OFFSET_X; Z = CP[2]; }
        case MOVE_BACK_L:   { X = CP[0]+(-OFFSET_X - OFFSET_Y); Y = CP[1]+(-OFFSET_Y + OFFSET_X); Z = CP[2]-OFFSET_Z; }
        case MOVE_BACK_R:   { X = CP[0]+(-OFFSET_X + OFFSET_Y); Y = CP[1]+(-OFFSET_Y - OFFSET_X); Z = CP[2]-OFFSET_Z; }
        case MOVE_FWD_L:    { X = CP[0]+(OFFSET_X  - OFFSET_Y); Y = CP[1]+(OFFSET_Y  + OFFSET_X); Z = CP[2]+OFFSET_Z; }
        case MOVE_FWD_R:    { X = CP[0]+(OFFSET_X  + OFFSET_Y); Y = CP[1]+(OFFSET_Y  - OFFSET_X); Z = CP[2]+OFFSET_Z; }
    }
    #undef OFFSET_X
    #undef OFFSET_Y
    #undef OFFSET_Z
}

stock FlyMode(playerid)
{
    new Float:X, Float:Y, Float:Z;

    IsCreating[playerid] = true;
    SettingFirstLoc[playerid] = true;

    GetPlayerPos(playerid, X, Y, Z);

    // Membuat objek invisible sebagai jangkar kamera
    noclipdata[playerid][flyobject] = CreatePlayerObject(playerid, 19300, X, Y, Z, 0.0, 0.0, 0.0);

    TogglePlayerSpectating(playerid, true);
    AttachCameraToPlayerObject(playerid, noclipdata[playerid][flyobject]);
 
    SetPVarInt(playerid, "FlyMode", 1);
    noclipdata[playerid][cameramode] = CAMERA_MODE_FLY;

    SendClientMessage(playerid, -1, "{8EFF8E}Camera Editor{FFFFFF} activated.");
    SendClientMessage(playerid, -1, "{FFFFFF}Gunakan {F58282}W, A, S, D{FFFFFF} untuk bergerak.");
    SendClientMessage(playerid, -1, "{FFFFFF}Tekan {F58282}LKM (FIRE){FFFFFF} untuk set {8EFF8E}START POINT{FFFFFF}.");
    return 1;
}

stock CancelFlyMode(playerid)
{
    DeletePVar(playerid, "FlyMode");
    CancelEdit(playerid);
    TogglePlayerSpectating(playerid, false);
    DestroyPlayerObject(playerid, noclipdata[playerid][flyobject]);

    noclipdata[playerid][cameramode] = CAMERA_MODE_NONE;
    IsReSettingStart[playerid]  = false;
    IsReSettingEnd[playerid]    = false;
    SettingFirstLoc[playerid]   = false;
    SettingLastLoc[playerid]    = false;
    return 1;
}

stock bool:IsStringNumeric(const szInput[]) 
{
    if(!szInput[0]) return false;
    for (new i = 0, l = strlen(szInput); i < l; i++) 
    {
        if (szInput[i] < '0' || szInput[i] > '9') return false;
    }
    return true;
}

stock ShowSpeedDialog(playerid)
{
    new string[512];
    format(string, sizeof(string), "{FFFFFF}Please enter the desired {F58282}movement{FFFFFF} duration in milliseconds\n\nCurrent movement speed: \t{F58282}%i ms\n{FFFFFF}Current rotation speed: \t{F58282}%i ms\n\n{CFCFCF}Note: 1000 ms = 1 detik", 
        coordInfo[playerid][MoveSpeed], coordInfo[playerid][RotSpeed]);
    
    ShowPlayerDialog(playerid, DIALOG_MOVE_SPEED, DIALOG_STYLE_INPUT, "{FFFFFF}Movement Duration Settings", string, "{8EFF8E}Confirm", "{F58282}Cancel");
    return 1;
}

stock GetMoveDirectionFromKeys(updown, leftright)
{
    new direction = 0;
    if (leftright < 0) // Tekan A (Kiri)
    {
        if (updown < 0) direction = MOVE_FWD_L;
        else if (updown > 0) direction = MOVE_BACK_L;
        else direction = MOVE_LEFT;
    }
    else if (leftright > 0) // Tekan D (Kanan)
    {
        if (updown < 0) direction = MOVE_FWD_R;
        else if (updown > 0) direction = MOVE_BACK_R;
        else direction = MOVE_RIGHT;
    }
    else if (updown < 0) direction = MOVE_FWD; // Tekan W (Maju)
    else if (updown > 0) direction = MOVE_BACK; // Tekan S (Mundur)

    return direction;
}
