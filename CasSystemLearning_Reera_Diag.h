#ifndef __CASSYSTEMLEARNING_REERA_DIAG_H
#define __CASSYSTEMLEARNING_REERA_DIAG_H

#include "rayanmessage.h"
#include "Algorithm.h"

class CasSystemLearning_Reera_Diag : public BaseAlgorithm
{
public:
	virtual bool InternalSwitchCase(CommandAdorner* Command);
	const static u16 Class_ID;
	CasSystemLearning_Reera_Diag(void);
	~CasSystemLearning_Reera_Diag(void);

	bool ProgramAntiThef(CommandAdorner* Command);
	bool ProgramPEPS(CommandAdorner* Command);
	bool LearningFOBKey(CommandAdorner* Command);
	bool DeletAllKeys(CommandAdorner* Command);
	bool ProgramESCL(CommandAdorner* Command);
	bool ProgramEngineECU(CommandAdorner* Command);
	bool NeutalizeECU(CommandAdorner* Command);
	bool Configuration(CommandAdorner* Command);
  bool Date_Configuration(CommandAdorner* Command);
	bool Remote_Learning(CommandAdorner* Command);
	bool ResetOdometer(CommandAdorner* Command);

private:
    bool AccessCodeEntered;
    char accessText[5];
    char fullCode[16];

    bool GetAccessCode();
    void ResetAccessCode();   

};
#endif //__CasSystemLearning_Reera_Diag_H
