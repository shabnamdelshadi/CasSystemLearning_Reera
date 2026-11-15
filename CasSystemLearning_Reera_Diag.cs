using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USILogicLayer.Protocol;
using Extensions.ByteArrayExtensions;
using DataAccessLayer;
using System.Threading;
using System.IO;
using Extensions.Loggers;
using USILogicLayer.Algorithms.Algorithm_Forms;
using System.Diagnostics;
using USILogicLayer.Algorithms;


namespace USILogicLayer.Algorithms
{
    public class CasSystemLearning_Reera_Diag : Algorithm
    {
        public CasSystemLearning_Reera_Diag(RayanDataModelDataSet.ECURow ECU, LogicManager logicManager)
                                                                            : base(ECU, logicManager)
        {
            Messages["CasSystemLearning_Reera_Diag_Canceled", Language.English] = "Operation canceled by user.";
            Messages["CasSystemLearning_Reera_Diag_Canceled", Language.Farsi] = "عملیات توسط کاربر لغو شد";

            Messages["CasSystemLearning_Reera_Diag_EnterDate", Language.English] = " EnterDate ";
            Messages["CasSystemLearning_Reera_Diag_EnterDate", Language.Farsi] = " تاریخ را وارد کنید ";

            Messages["CasSystemLearning_Reera_Diag_ECU_done", Language.English] = " Operation done ";
            Messages["CasSystemLearning_Reera_Diag_ECU_done", Language.Farsi] = " مقادیر وارد شده به درستی ثبت شد ";

            Messages["CasSystemLearning_Reera_Diag_OperationFailed", Language.English] = " Operation failed! ";
            Messages["CasSystemLearning_Reera_Diag_OperationFailed", Language.Farsi] = " عملیات با موفقیت انجام نشد! ";

            Messages["CasSystemLearning_Reera_Diag_tryagain", Language.English] = "Try again later";
            Messages["CasSystemLearning_Reera_Diag_tryagain", Language.Farsi] = "در فرصتی دیگر دوباره تلاش نمایید";

            Messages["CasSystemLearning_Reera_Diag_EnterRemoteCount", Language.English] = " Enter the number of remotes, from 1 to 4. ";
            Messages["CasSystemLearning_Reera_Diag_EnterRemoteCount", Language.Farsi] = " تعداد ریموت را وارد کنید، 1 تا 4 عدد. ";

            Messages["CasSystemLearning_Reera_Diag_resetodo", Language.English] = "ODO Value must be lass than 100km\r\nODO Value has not reset before";
            Messages["CasSystemLearning_Reera_Diag_resetodo", Language.Farsi] = "میزان پیمایش باید کم تر از 100 کیلومتر باشد\r\nمیزان پیمایش نباید از قبل ریست شده باشد";

            Messages["CasSystemLearning_Reera_Diag_RemoteDefinedSuccessfully", Language.English] = " The remote was defined successfully. ";
            Messages["CasSystemLearning_Reera_Diag_RemoteDefinedSuccessfully", Language.Farsi] = " ریموت با موفقیت تعریف شد. ";

            Messages["CasSystemLearning_Reera_Diag_PressRemoteButton", Language.English] = " Please press one of the first remote buttons.\r\nIn some cases, it is necessary to hold two remote keys simultaneously for 4 seconds. ";
            Messages["CasSystemLearning_Reera_Diag_PressRemoteButton", Language.Farsi] = " لطفا یکی از دکمه های ریموت اول را بزنید.\r\nدر بعضی از موارد نیاز است تا دو کلید ریموت به طور همزمان و به مدت 4 ثانیه نگه داشته شوند. ";

            Messages["CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned", Language.English] = " The ignition switch must be off\r\n, and the remote must be blank or previously learned to this vehicle\r\n, and the ECU must be blank or previously learned to this vehicle. ";
            Messages["CasSystemLearning_Reera_Diag_SwitchOff_RemoteBlankOrLearned_EcuBlankOrLearned", Language.Farsi] = " سوییچ باید بسته باشد\r\n و ریموت باید خام باشد یا قبلاً در این خودرو تعریف شده باشد\r\n و ایسیو نیز باید خام باشد یا قبلاً در این خودرو تعریف شده باشد. ";

            Messages["CasSystemLearning_Reera_Diag_UpTo5KeysSupported", Language.English] = " This system supports defining up to 5 keys. ";
            Messages["CasSystemLearning_Reera_Diag_UpTo5KeysSupported", Language.Farsi] = " این سیستم قابلیت تعریف تا 5 کلید را دارد. ";

            Messages["CasSystemLearning_Reera_Diag_PlaceKeyInFrontOfGearShifter", Language.English] = " Place the key in front of the gear shifter _inside the cup holder_. ";
            Messages["CasSystemLearning_Reera_Diag_PlaceKeyInFrontOfGearShifter", Language.Farsi] = " کلید را در جلوی دسته دنده (داخل جای لیوانی) قرار دهید. ";

            Messages["CasSystemLearning_Reera_Diag_Completed", Language.English] = "Completed successfully.";
            Messages["CasSystemLearning_Reera_Diag_Completed", Language.Farsi] = "انجام شد";

            Messages["CasSystemLearning_Reera_Diag_SwitchNotClosed", Language.English] = "Switch is not closed.";
            Messages["CasSystemLearning_Reera_Diag_SwitchNotClosed", Language.Farsi] = "سوییچ بسته نمیباشد";

            Messages["CasSystemLearning_Reera_Diag_VehicleNetworkAbnormal", Language.English] = "Vehicle network is not normal.";
            Messages["CasSystemLearning_Reera_Diag_VehicleNetworkAbnormal", Language.Farsi] = "شبکه خودرو نرمال نمیباشد";

            Messages["CasSystemLearning_Reera_Diag_PEPSDefinedAndLocked", Language.English] = "PEPS is programmed and locked.";
            Messages["CasSystemLearning_Reera_Diag_PEPSDefinedAndLocked", Language.Farsi] = "پپس تعریف شده و لاک میباشد";

            Messages["CasSystemLearning_Reera_Diag_PEPSNotDefinedButLocked", Language.English] = "PEPS is not programmed but is locked.";
            Messages["CasSystemLearning_Reera_Diag_PEPSNotDefinedButLocked", Language.Farsi] = "پپس تعریف نشده ولی لاک است";

            Messages["CasSystemLearning_Reera_Diag_PEPSAntiScan", Language.English] = "PEPS is in anti-scan mode due to an incorrect key.";
            Messages["CasSystemLearning_Reera_Diag_PEPSAntiScan", Language.Farsi] = "پپس به دلیل وارد کردن سوییچ اشتباه در حالت آنتی اسکن است";

            Messages["CasSystemLearning_Reera_Diag_IncorrectKeyCode", Language.English] = "Incorrect key code entered.";
            Messages["CasSystemLearning_Reera_Diag_IncorrectKeyCode", Language.Farsi] = "کد سوییچ اشتباه وارد شده است";

            Messages["CasSystemLearning_Reera_Diag_NUMError", Language.English] = "NUM error. Replace PEPS if the error repeats.";
            Messages["CasSystemLearning_Reera_Diag_NUMError", Language.Farsi] = "خطای NUM. در صورت تکرار پپس تعویض شود";

            Messages["CasSystemLearning_Reera_Diag_KeyNotRegisteredInPEPS", Language.English] = "Key is not registered in PEPS.";
            Messages["CasSystemLearning_Reera_Diag_KeyNotRegisteredInPEPS", Language.Farsi] = "سوییچ در یونیت پپس ثبت نشده است";

            Messages["CasSystemLearning_Reera_Diag_VINNotRegisteredInPEPS", Language.English] = "Vehicle identification number is not registered in PEPS.";
            Messages["CasSystemLearning_Reera_Diag_VINNotRegisteredInPEPS", Language.Farsi] = "کد شناسه خودرو در پپس ثبت نشده است";

            Messages["CasSystemLearning_Reera_Diag_MaxKeysExceeded", Language.English] = "Maximum number of keys exceeded.";
            Messages["CasSystemLearning_Reera_Diag_MaxKeysExceeded", Language.Farsi] = "تعداد کلیدهای تعریف شده بیش از حد مجاز است";

            Messages["CasSystemLearning_Reera_Diag_AntiTheftBaseSystemError", Language.English] = "Base system error. Check the anti-theft system and the starter unit.";
            Messages["CasSystemLearning_Reera_Diag_AntiTheftBaseSystemError", Language.Farsi] = "خطای بیس سیستم ضد سرقت. یونیت استارت چک شود";

            Messages["CasSystemLearning_Reera_Diag_BlankKey", Language.English] = "Key is blank.";
            Messages["CasSystemLearning_Reera_Diag_BlankKey", Language.Farsi] = "کلید خام میباشد";

            Messages["CasSystemLearning_Reera_Diag_KeyNoResponse", Language.English] = "No response from key.";
            Messages["CasSystemLearning_Reera_Diag_KeyNoResponse", Language.Farsi] = "کلید پاسخگو نمیباشد";

            Messages["CasSystemLearning_Reera_Diag_ESCLNotUnlocked", Language.English] = "Electronic steering column lock is not unlocked.";
            Messages["CasSystemLearning_Reera_Diag_ESCLNotUnlocked", Language.Farsi] = "یونیت قفل فرمان باز نمیباشد";

            Messages["CasSystemLearning_Reera_Diag_ESCLNegativeResponse", Language.English] = "Negative response from the electronic steering column lock.";
            Messages["CasSystemLearning_Reera_Diag_ESCLNegativeResponse", Language.Farsi] = "دریافت پاسخ منفی از قفل برقی فرمان";

            Messages["CasSystemLearning_Reera_Diag_ESCLNoResponse", Language.English] = "No response from ESCL.";
            Messages["CasSystemLearning_Reera_Diag_ESCLNoResponse", Language.Farsi] = "عدم پاسخگویی ESCL";

            Messages["CasSystemLearning_Reera_Diag_EngineECUNoResponse", Language.English] = "No response from the engine ECU.";
            Messages["CasSystemLearning_Reera_Diag_EngineECUNoResponse", Language.Farsi] = "عدم پاسخگویی ایسیو موتور";

            Messages["CasSystemLearning_Reera_Diag_TPMSNoResponse", Language.English] = "No response from the TPMS sensor.";
            Messages["CasSystemLearning_Reera_Diag_TPMSNoResponse", Language.Farsi] = "عدم پاسخگویی سنسور TPMS";

            Messages["CasSystemLearning_Reera_Diag_OperationFailed", Language.English] = "Operation failed.";
            Messages["CasSystemLearning_Reera_Diag_OperationFailed", Language.Farsi] = "عملیات انجام نشد";

            Messages["CasSystemLearning_Reera_Diag_BlankECU", Language.English] = "ECU is blank.";
            Messages["CasSystemLearning_Reera_Diag_BlankECU", Language.Farsi] = "ایسیو خام است";
            
            Messages["CasSystemLearning_Reera_Diag_Preconditions", Language.English] = "Make sure the switch is closed, the vehicle network is normal, PEPS is learned, and the engine ECU is learned.";
            Messages["CasSystemLearning_Reera_Diag_Preconditions", Language.Farsi] = "سوییچ بسته باشد، شبکه خودرو نرمال باشد، پپس تعریف باشد و ایسیو موتور تعریف باشد";

            Messages["CasSystemLearning_Reera_Diag_NotBlankECU", Language.English] = "ECU is not blank.";
            Messages["CasSystemLearning_Reera_Diag_NotBlankECU", Language.Farsi] = "ایسیو خام نمی‌باشد";

            Messages["CasSystemLearning_Reera_Diag_DefinedWithAnotherSwitchCode", Language.English] = "The vehicle's ECU is defined with a different switch code.";
            Messages["CasSystemLearning_Reera_Diag_DefinedWithAnotherSwitchCode", Language.Farsi] = "ایسیو ماشین با کد سوئیچ دیگری تعریف شده است";

            Messages["CasSystemLearning_Reera_Diag_Precondition", Language.English] = "Make sure the switch is closed, the vehicle network is normal, PEPS is learned, and the engine ECU is blank.";
            Messages["CasSystemLearning_Reera_Diag_Precondition", Language.Farsi] = "سوییچ بسته باشد، شبکه خودرو نرمال باشد، پپس تعریف باشد و ایسیو موتور خام باشد";

            Messages["CasSystemLearning_Reera_Diag_ECUNotConnected", Language.English] = "ECU is not connected.";
            Messages["CasSystemLearning_Reera_Diag_ECUNotConnected", Language.Farsi] = "ایسیو متصل نمی‌باشد";

            Messages["CasSystemLearning_Reera_Diag_Preconditions1", Language.English] = "Ensure the switch is closed, the vehicle network is normal, PEPS is learned, and the Electric Steering Column Lock (ESCL) is blank.";
            Messages["CasSystemLearning_Reera_Diag_Preconditions1", Language.Farsi] = "سوییچ بسته باشد، شبکه خودرو نرمال باشد، تعریف پپس انجام شده باشد و قفل برقی فرمان خام باشد.";

            Messages["CasSystemLearning_Reera_Diag_Preconditions", Language.English] = "Ensure the switch is closed and PEPS is learned.";
            Messages["CasSystemLearning_Reera_Diag_Preconditions", Language.Farsi] = "سوییچ بسته باشد و پپس تعریف شده باشد";

            Messages["CasSystemLearning_Reera_Diag_Preconditions2", Language.English] = "Already defined.";
            Messages["CasSystemLearning_Reera_Diag_Preconditions2", Language.Farsi] = "قبلا تعریف شده است";

            Messages["CasSystemLearning_Reera_Diag_EnterNumberOfKeys", Language.English] = "Enter Number Of Keys";
            Messages["CasSystemLearning_Reera_Diag_EnterNumberOfKeys", Language.Farsi] = "تعداد کلید را وارد کنید";

            Messages["CasSystemLearning_Reera_Diag_EnterSwitchCode", Language.English] = "Enter Switch Code";
            Messages["CasSystemLearning_Reera_Diag_EnterSwitchCode", Language.Farsi] = "کد سوییچ را وارد کنید";

            Messages["CasSystemLearning_Reera_Diag_IsVinCorrect", Language.English] = "Is VIN correct?";
            Messages["CasSystemLearning_Reera_Diag_IsVinCorrect", Language.Farsi] = "آیا VIN درست است؟";

            Messages["CasSystemLearning_Reera_Diag_VinInvalid", Language.English] = "VIN must be exactly 17 characters (letters and numbers only). Please retry.";
            Messages["CasSystemLearning_Reera_Diag_VinInvalid", Language.Farsi] = "VIN باید دقیقاً 17 کاراکتر باشد (فقط حروف و اعداد). لطفاً دوباره تلاش کنید.";
        }
    }
}