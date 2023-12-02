#include <string>
/*
* @details CanData保管構造体
*/
class CanData
{
	public:
		std::string strTime;
		std::string strMsgID;
		std::string strDLC;
		std::string strData;
    /**
	 * @details コンストラクタ
	*/
    CanData()
    {
        clear();
    }
    
	// CanDataのメンバが空か判定する
	bool isNull()
	{
		bool blRet = false;
		if (strTime.empty() && strMsgID.empty() && strDLC.empty() && strData.empty())
		{
			blRet = true;
		}
		return blRet;
	}
	
	/*
	*@details メンバの初期化
	*/
    void clear()
    {
		strTime = "";
		strMsgID = "";
		strDLC = "";
		strData = "";
    }
};