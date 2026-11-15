#include "CasSystemLearning_Reera_Diag.h" //Delshadi
#include "MSG_CasSystemLearning_Reera_Diag.h"
#include "ArrayTools.h"
#include "OtherHeader\CasSystemLearning_Reera_Diag_OH.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CasSystemLearning_Reera_Diag::CasSystemLearning_Reera_Diag(void)
{
    AccessCodeEntered = false;
    memset(accessText, 0, sizeof(accessText));
    memset(fullCode, 0, sizeof(fullCode));
}
CasSystemLearning_Reera_Diag::~CasSystemLearning_Reera_Diag(void)
{
    ResetAccessCode();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CasSystemLearning_Reera_Diag::ResetAccessCode()
{
    AccessCodeEntered = false;
    memset(accessText, 0, sizeof(accessText));
    memset(fullCode, 0, sizeof(fullCode));
}

bool CasSystemLearning_Reera_Diag::GetAccessCode()
{
    if (AccessCodeEntered)
        return true;
    memset(accessText, 0, sizeof(accessText));
    u8 Output = (u8)GetTextDataDialog(Messages(CasSystemLearning_Reera_Diag_EnterSwitchCode), 4, accessText);
    if (accessText[0] == 0 || accessText[1] == 0 || accessText[2] == 0 || accessText[3] == 0)
    {
        ResetAccessCode();
        return false;
    }
    snprintf(fullCode, sizeof(fullCode), "K1H100%04X", Output);
    AccessCodeEntered = true;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::Configuration(CommandAdorner *Command)
{
    if (!GetAccessCode())
        return false;
    ResetAccessCode();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CasSystemLearning_Reera_Diag::ProgramAntiThef(CommandAdorner *Command) //تعریف کل ضد سرقت 
{
    if (!OBDii.Security_Pass(Command, 1))
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        return false;
    }
    if (!GetAccessCode())
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        return false;
    }
    byte response[255] = {};
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_UpTo5KeysSupported, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    byte counter = (u8)(GetDecimalDataDialog(Messages(CasSystemLearning_Reera_Diag_EnterNumberOfKeys), 1, 5));
    if (counter < 1 || counter > 5)
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return false;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PlaceKeyInFrontOfGearShifter, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        if (sendFrame == null)
        {
            Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
            return false;
        }
        if (i == 5 || i == 10)
        {
            if (!OBDii.Security_Pass(Command, 1))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        if (i == 5)
        {
            byte send_buff[7] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2],accessText[0], accessText[1], accessText[2], accessText[3]};
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
		else if (i == 6)
		{
				byte VIN_Request[3] = {0x22, 0xF1, 0x90};
				if ((OBDii.Send_Receive3(sizeof(VIN_Request), VIN_Request, response, sizeof(response), 0x749, 0x70A, MainDB.ECUData.DB->CommunicationType) == null) || (response[0] != 0x62))
				{
						Manager.BreakFlag = true;
						Command->State.IsFaulty = true;
						return false;
				}
				char vin[18] = {0};
				for (byte k = 0; k < 17; ++k)
						vin[k] = (char)response[3 + k];
				vin[17] = '\0';
				char msg[255] = {0};
				snprintf(msg, sizeof(msg), "%s %s", Messages(CasSystemLearning_Reera_Diag_IsVinCorrect), vin);
				char vinToWrite[18] = {0};
				memcpy(vinToWrite, vin, 17);
				if (!OnShowMessage(msg, Yes_No, false))
				{
						bool valid = false;
						while (!valid)
						{
								for (u8 b = 0; b < 18; b++) vinToWrite[b] = 0;
								u8 Output = (u8)GetTextDataDialog("VIN", 17, vinToWrite);
								if (Output == 0)
								{
										OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
										return true;
								}
								valid = true;
								if (strlen(vinToWrite) != 17)
										valid = false;
								if (valid)
								{
										bool is00 = (vinToWrite[0] == '0' && vinToWrite[1] == '0');
										bool isFF = ((vinToWrite[0] == 'f' || vinToWrite[0] == 'F') && (vinToWrite[1] == 'f' || vinToWrite[1] == 'F'));
										if (is00 || isFF)
												valid = false;
								}
								if (valid)
								{
										for (u8 k = 0; k < 17; k++)
										{
												u8 c = (u8)vinToWrite[k];
												if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
												{ valid = false; break; }
												if (c >= 'a' && c <= 'z') vinToWrite[k] = c - ('a' - 'A');
										}
								}
								if (!valid)
										OnShowMessage(Messages(CasSystemLearning_Reera_Diag_VinInvalid), Yes, true);
						}
				}
				byte frame6[20] = {0x2E, 0xF1, 0x90};
				memcpy(&frame6[3], vinToWrite, 17);
				if ((OBDii.Send_Receive3(sizeof(frame6), frame6, response, sizeof(response), 0x749, 0x70A, MainDB.ECUData.DB->CommunicationType) == null)|| (response[0] != 0x6E))
				{
						Manager.BreakFlag = true;
						Command->State.IsFaulty = true;
						return false;
				}
		}
        else if (i == 7)
        {
            byte send_buff[8] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3]};
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        else if (i == 9)
        {
            for (byte j = 1; j <= counter; ++j)
            {
                byte send_buff[8] = {
                    sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3] };
                if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
                CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame10 = Command_SendingFrames(Command, 10);
                if (sendFrame10 == null)
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
                if ((OBDii.Send_Receive(sendFrame10->Lengh, sendFrame10->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
            }
        }
        else if (i == 10)
        {
            continue;
        }
        else if (i == 13)
        {
            byte send_buff[8] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3] };
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        else
        {
            if ((OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::ProgramPEPS(CommandAdorner *Command) // تعریف یونیت PEPS
{
     if (!OBDii.Security_Pass(Command, 1))
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        return false;
    }
    if (!GetAccessCode())
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        return false;
    }
    byte response[255] = {};
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_UpTo5KeysSupported, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    byte counter = (u8)(GetDecimalDataDialog(Messages(CasSystemLearning_Reera_Diag_EnterNumberOfKeys), 1, 5));
    if (counter < 1 || counter > 5)
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return false;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PlaceKeyInFrontOfGearShifter, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        if (sendFrame == null)
        {
            Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
            return false;
        }
        if (i == 5 || i == 10)
        {
            if (!OBDii.Security_Pass(Command, 1))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        if (i == 5)
        {
            byte send_buff[7] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2],accessText[0], accessText[1], accessText[2], accessText[3]};
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
		else if (i == 6)
		{
				byte VIN_Request[3] = {0x22, 0xF1, 0x90};
				if ((OBDii.Send_Receive3(sizeof(VIN_Request), VIN_Request, response, sizeof(response), 0x749, 0x70A, MainDB.ECUData.DB->CommunicationType) == null) || (response[0] != 0x62))
				{
						Manager.BreakFlag = true;
						Command->State.IsFaulty = true;
						return false;
				}
				char vin[18] = {0};
				for (byte k = 0; k < 17; ++k)
						vin[k] = (char)response[3 + k];
				vin[17] = '\0';
				char msg[255] = {0};
				snprintf(msg, sizeof(msg), "%s %s", Messages(CasSystemLearning_Reera_Diag_IsVinCorrect), vin);
				char vinToWrite[18] = {0};
				memcpy(vinToWrite, vin, 17);
				if (!OnShowMessage(msg, Yes_No, false))
				{
						bool valid = false;
						while (!valid)
						{
								for (u8 b = 0; b < 18; b++) vinToWrite[b] = 0;
								u8 Output = (u8)GetTextDataDialog("VIN", 17, vinToWrite);
								if (Output == 0)
								{
										OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
										return true;
								}
								valid = true;
								if (strlen(vinToWrite) != 17)
										valid = false;
								if (valid)
								{
										bool is00 = (vinToWrite[0] == '0' && vinToWrite[1] == '0');
										bool isFF = ((vinToWrite[0] == 'f' || vinToWrite[0] == 'F') && (vinToWrite[1] == 'f' || vinToWrite[1] == 'F'));
										if (is00 || isFF)
												valid = false;
								}
								if (valid)
								{
										for (u8 k = 0; k < 17; k++)
										{
												u8 c = (u8)vinToWrite[k];
												if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
												{ valid = false; break; }
												if (c >= 'a' && c <= 'z') vinToWrite[k] = c - ('a' - 'A');
										}
								}
								if (!valid)
										OnShowMessage(Messages(CasSystemLearning_Reera_Diag_VinInvalid), Yes, true);
						}
				}
				byte frame6[20] = {0x2E, 0xF1, 0x90};
				memcpy(&frame6[3], vinToWrite, 17);
				if ((OBDii.Send_Receive3(sizeof(frame6), frame6, response, sizeof(response), 0x749, 0x70A, MainDB.ECUData.DB->CommunicationType) == null)|| (response[0] != 0x6E))
				{
						Manager.BreakFlag = true;
						Command->State.IsFaulty = true;
						return false;
				}
		}
        else if (i == 7)
        {
            byte send_buff[8] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3]};
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        else if (i == 9)
        {
            for (byte j = 1; j <= counter; ++j)
            {
                byte send_buff[8] = {
                    sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3] };
                if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
                CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame10 = Command_SendingFrames(Command, 10);
                if (sendFrame10 == null)
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
                if ((OBDii.Send_Receive(sendFrame10->Lengh, sendFrame10->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
                {
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                    return false;
                }
            }
        }
        else if (i == 10)
        {
            continue;
        }
        else if (i == 13)
        {
            byte send_buff[8] = { sendFrame->ByteFrame[0], sendFrame->ByteFrame[1], sendFrame->ByteFrame[2], sendFrame->ByteFrame[3], accessText[0], accessText[1], accessText[2], accessText[3] };
            if ((OBDii.Send_Receive(sizeof(send_buff), send_buff, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
        else
        {
            if ((OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
                return false;
            }
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::LearningFOBKey(CommandAdorner *Command) // تعریف کلید
{
    if (!OBDii.Security_Pass(Command, 1))
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }
    if (!GetAccessCode())
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }
    byte response[255] = {};
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Preconditions, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_UpTo5KeysSupported, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    byte counter = (u8)(GetDecimalDataDialog(Messages(CasSystemLearning_Reera_Diag_EnterNumberOfKeys), 1, 5));

    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        if ((OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
        {
            Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
        }
        if (i == 3)
        {
            if (response[5] % 2 == 1)
            {
                for (byte j = 1; j <= counter; ++j)
                {
                    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PlaceKeyInFrontOfGearShifter, Yes_No, true))
                    {
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
                        return true;
                    }
                    byte frame3[8] = {0x31, 0x01, 0x40, 0x04, accessText[0], accessText[1], accessText[2], accessText[3]};
                    OBDii.Send_Receive(sizeof(frame3), frame3, response, sizeof(response));
                    byte frame4[4] = {0x31, 0x03, 0x40, 0x04};
                    OBDii.Send_Receive(sizeof(frame4), frame4, response, sizeof(response));
                }
                switch (response[4])
                {
                case 0:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Completed, Yes, true);
                    break;
                case 1:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchNotClosed, Yes, true);
                    break;
                case 2:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_VehicleNetworkAbnormal, Yes, true);
                    break;
                case 3:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSDefinedAndLocked, Yes, true);
                    break;
                case 4:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSNotDefinedButLocked, Yes, true);
                    break;
                case 5:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSAntiScan, Yes, true);
                    break;
                case 6:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_IncorrectKeyCode, Yes, true);
                    break;
                case 7:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_NUMError, Yes, true);
                    break;
                case 8:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_KeyNotRegisteredInPEPS, Yes, true);
                    break;
                case 9:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_VINNotRegisteredInPEPS, Yes, true);
                    break;
                case 10:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_MaxKeysExceeded, Yes, true);
                    break;
                case 11:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_AntiTheftBaseSystemError, Yes, true);
                    break;
                case 12:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_BlankKey, Yes, true);
                    break;
                case 13:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_KeyNoResponse, Yes, true);
                    break;
                case 14:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNotUnlocked, Yes, true);
                    break;
                case 15:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNegativeResponse, Yes, true);
                    break;
                case 16:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNoResponse, Yes, true);
                    break;
                case 17:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_NotBlankECU, Yes, true);
                    break;
                case 18:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_DefinedWithAnotherSwitchCode, Yes, true);
                    break;
                case 19:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNegativeResponse, Yes, true);
                    break;
                case 20:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_EngineECUNoResponse, Yes, true);
                    break;
                case 21:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_TPMSNoResponse, Yes, true);
                    break;
                case 22:
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_OperationFailed, Yes, true);
                    break;
                default:
                    Manager.BreakFlag = true;
                    Command->State.IsFaulty = true;
                }
            }
            else
                Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::DeletAllKeys(CommandAdorner *Command) // پاک کردن همه ی کلید ها
{
    if (!OBDii.Security_Pass(Command, 1))
        Manager.BreakFlag = true;
    Command->State.IsFaulty = true;
    if (!GetAccessCode())
        Manager.BreakFlag = true;
    Command->State.IsFaulty = true;
    byte response[255] = {};
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Preconditions, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        if ((OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response)) == null) || (response[0] == 0x7F))
            Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
        if (i == 3)
        {
            if (response[6] != 0x00)
            {
                byte frame1[8] = {0x31, 0x01, 0x40, 0x08, accessText[0], accessText[1], accessText[2], accessText[3]};
                OBDii.Send_Receive(sizeof(frame1), frame1, response, sizeof(response));
                byte frame2[4] = {0x31, 0x03, 0x40, 0x08};
                OBDii.Send_Receive(sizeof(frame2), frame2, response, sizeof(response));
            }
            else
                Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
        }
    }
    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Completed, Yes, true);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::ProgramESCL(CommandAdorner *Command) // تعریف قفل برقی فرمان
{
    if (!OBDii.Security_Pass(Command, 1))
        Manager.BreakFlag = true;
    Command->State.IsFaulty = true;

    if (!GetAccessCode())
        Manager.BreakFlag = true;
    Command->State.IsFaulty = true;

    byte response[255] = {};

    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Preconditions1, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response));
        if (i == 3)
        {
            if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Preconditions2, Yes_No, true))
            {
                OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
                return true;
            }
            if (response[5] % 2 == 1)
            {
                byte frame1[4] = {0x31, 0x01, 0x40, 0x01};
                OBDii.Send_Receive(sizeof(frame1), frame1, response, sizeof(response));
                byte frame2[4] = {0x31, 0x03, 0x40, 0x01};
                OBDii.Send_Receive(sizeof(frame2), frame2, response, sizeof(response));
                if (response[5] % 2 == 1)
                {
                    byte frame3[8] = {0x31, 0x01, 0x40, 0x05, accessText[0], accessText[1], accessText[2], accessText[3]};
                    OBDii.Send_Receive(sizeof(frame3), frame3, response, sizeof(response));
                    byte frame4[4] = {0x31, 0x01, 0x40, 0x05};
                    OBDii.Send_Receive(sizeof(frame4), frame4, response, sizeof(response));
                    byte frame5[2] = {0x10, 0x01};
                    OBDii.Send_Receive(sizeof(frame5), frame5, response, sizeof(response));
                    byte frame6[2] = {0x10, 0x03};
                    OBDii.Send_Receive(sizeof(frame6), frame6, response, sizeof(response));
                }
                else
                    Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
            }
            else
                Manager.BreakFlag = true;
            Command->State.IsFaulty = true;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::ProgramEngineECU(CommandAdorner *Command) // تعریف ایسیوموتور
{
    if (!OBDii.Security_Pass(Command, 1))
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }
    if (!GetAccessCode())
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }
    byte response[255] = {};
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Precondition, Yes_No, true))
    {
        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
        return true;
    }
    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response));
        if (i == 5)
        {
            if (response[5] == 2)
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
            }
            else if (response[5] == 1)
            {
                byte frame1[8] = {0x31, 0x01, 0x40, 0x07, accessText[0], accessText[1], accessText[2], accessText[3]};
                OBDii.Send_Receive(sizeof(frame1), frame1, response, sizeof(response));
                byte frame2[4] = {0x31, 0x03, 0x40, 0x07};
                OBDii.Send_Receive(sizeof(frame2), frame2, response, sizeof(response));
            }
            else
                OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Completed, Yes, true);
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CasSystemLearning_Reera_Diag::NeutalizeECU(CommandAdorner *Command) // خنثی سای ایسیو
{
    if (!OBDii.Security_Pass(Command, 1))
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }

    if (!GetAccessCode())
    {
        Manager.BreakFlag = true;
        Command->State.IsFaulty = true;
    }

    byte response[255] = {};

    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Preconditions, Yes_No, true);

    for (byte i = 2; i < Command->DB->LengthOfSendingFrames; i++)
    {
        CommandAdorner::SendingFrame_ByteArrayFrame_structure *sendFrame = Command_SendingFrames(Command, i);
        OBDii.Send_Receive(sendFrame->Lengh, sendFrame->ByteFrame, response, sizeof(response));

        if (i == 3)
        {
            if (response[4] == 0)
            {
                if (!OnShowMessage_Messages(CasSystemLearning_Reera_Diag_BlankECU, Yes_No, true))
                {
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Canceled, Yes, true);
                    return true;
                }
                if (response[5] == 1)
                {
                    OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Completed, Yes, true);
                    byte frame1[8] = {0x31, 0x01, 0x40, 0x09, accessText[0], accessText[1], accessText[2], accessText[3]};
                    OBDii.Send_Receive(sizeof(frame1), frame1, response, sizeof(response));
                    byte frame2[4] = {0x31, 0x03, 0x40, 0x09};
                    OBDii.Send_Receive(sizeof(frame2), frame2, response, sizeof(response));
                    switch (response[4])
                    {
                    case 0:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_Completed, Yes, true);
                        break;
                    case 1:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_SwitchNotClosed, Yes, true);
                        break;
                    case 2:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_VehicleNetworkAbnormal, Yes, true);
                        break;
                    case 3:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSDefinedAndLocked, Yes, true);
                        break;
                    case 4:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSNotDefinedButLocked, Yes, true);
                        break;
                    case 5:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_PEPSAntiScan, Yes, true);
                        break;
                    case 6:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_IncorrectKeyCode, Yes, true);
                        break;
                    case 7:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_NUMError, Yes, true);
                        break;
                    case 8:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_KeyNotRegisteredInPEPS, Yes, true);
                        break;
                    case 9:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_VINNotRegisteredInPEPS, Yes, true);
                        break;
                    case 10:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_MaxKeysExceeded, Yes, true);
                        break;
                    case 11:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_AntiTheftBaseSystemError, Yes, true);
                        break;
                    case 12:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_BlankKey, Yes, true);
                        break;
                    case 13:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_KeyNoResponse, Yes, true);
                        break;
                    case 14:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNotUnlocked, Yes, true);
                        break;
                    case 15:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNegativeResponse, Yes, true);
                        break;
                    case 16:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNoResponse, Yes, true);
                        break;
                    case 17:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_NotBlankECU, Yes, true);
                        break;
                    case 18:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_DefinedWithAnotherSwitchCode, Yes, true);
                        break;
                    case 19:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_ESCLNegativeResponse, Yes, true);
                        break;
                    case 20:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_EngineECUNoResponse, Yes, true);
                        break;
                    case 21:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_TPMSNoResponse, Yes, true);
                        break;
                    case 22:
                        OnShowMessage_Messages(CasSystemLearning_Reera_Diag_OperationFailed, Yes, true);
                        break;
                    default:
                        Manager.BreakFlag = true;
                        Command->State.IsFaulty = true;
                        return true;
                    }
                }
            }
            else
            {
                Manager.BreakFlag = true;
                Command->State.IsFaulty = true;
            }
            byte frame3[2] = {0x10, 0x01};
            OBDii.Send_Receive(sizeof(frame3), frame3, response, sizeof(response));
            byte frame4[2] = {0x10, 0x03};
            OBDii.Send_Receive(sizeof(frame4), frame4, response, sizeof(response));
        }
    }
    return true;
}
