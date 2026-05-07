#pragma once
#include <string>

class CreateTableClubRequest
{
    friend class CreateTableClubRespons;
public:
    CreateTableClubRequest() {}
    CreateTableClubRequest(const std::string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, lUid);
        SERIALIZE_MEMBER(writer, lClubID);
        SERIALIZE_MEMBER(writer, lGameType);
        SERIALIZE_MEMBER(writer, lTimeId);
        SERIALIZE_MEMBER(writer, lMaxSeat);
        SERIALIZE_MEMBER(writer, lSpeedType);
        SERIALIZE_MEMBER(writer, lBlindId);
        SERIALIZE_MEMBER(writer, lPet);
        SERIALIZE_MEMBER(writer, lWatch);
        writer.EndObject();
    }

    void toString(std::string &json)
    {
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        Serialize(writer);
        json = sb.GetString();
    }

    void Deserialize(const string &json)
    {
        try
        {
            Document d;
            if (d.Parse(json.c_str()).HasParseError())
            {
                throw logic_error("parse json error. raw data : " + json);
            }
            SET_DOC_MEMBER(d, lUid);
            SET_DOC_MEMBER(d, lClubID);
            SET_DOC_MEMBER(d, lGameType);
            SET_DOC_MEMBER(d, lTimeId);
            SET_DOC_MEMBER(d, lMaxSeat);
            SET_DOC_MEMBER(d, lSpeedType);
            SET_DOC_MEMBER(d, lBlindId);
            SET_DOC_MEMBER(d, lPet);
            SET_DOC_MEMBER(d, lWatch);
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":CreateTableClubRequest decode error!");
            throw logic_error(errInfo);
        }
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        return 0;
    }

private:
    CInteger _lUid;         // 用户UID
    CInteger _lClubID;      // 俱乐部ID
    CInteger _lGameType;    // 玩法类型 1 经典 2 AOF 3 短牌
    CInteger _lTimeId;      // 房间时间ID
    CInteger _lMaxSeat;     // 最大座位数 2-6-9
    CInteger _lSpeedType;   // 速度类型 1 快速 2 常规
    CInteger _lBlindId;     // 肓注ID
    CInteger _lPet;         // 抽水百分比
    CInteger _lWatch;       // 是否允许非俱乐部成员观点 0 否 1 是
};
class CreateTableClubRespons
{
public:
    CreateTableClubRespons() {}
    CreateTableClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        writer.EndObject();
    }

    void toString(std::string &json)
    {
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        Serialize(writer);
        json = sb.GetString();
    }

    void Deserialize(const string &json)
    {
        Document d;
        if (d.Parse(json.c_str()).HasParseError())
        {
            throw logic_error("parse json error. raw data : " + json);
        }
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        CreateTableClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
        string sServantPrx;
        int ret = g_app.getOuterFactoryPtr()->getRoomServerPrx("1:1001:22013900", sServantPrx);
        if (ret != 0)
        {
            ROLLLOG_ERROR << "get room prx err , ret: " << ret << endl;
            resultCode = RESULT_CODE_FAIL;
        }

        auto pServantObj = Application::getCommunicator()->stringToProxy<JFGame::RoomServantPrx>(sServantPrx);
        if (!pServantObj)
        {
            resultCode = RESULT_CODE_FAIL;
            ret = -2;
        }

        string sRoomKey = "";
        if(ret == 0)
        {
            java2room::CreateTableReq req;
            java2room::CreateTableResp resp;
            req.uId = request._lUid;
            req.clubId = request._lClubID;
            req.gameType = request._lGameType;
            req.timeId = request._lTimeId;
            req.maxSeat = request._lMaxSeat;
            req.speedType = request._lSpeedType;
            req.blindId = request._lBlindId;
            req.pet = request._lPet;
            req.insure = 1;
            req.watch = request._lWatch;

            ROLLLOG_DEBUG << "CreateTableReq: " << printTars(req) << endl;

            int ret = pServantObj->onCreatePrTable(req, resp);
            if(ret != 0)
            {
                resultCode = RESULT_CODE_FAIL;
                ret = -3;
            }

            resultCode = resp.resultCode;
            sRoomKey = resp.roomKey;
            ROLLLOG_DEBUG << "CreateTableReq: " << printTars(resp) << endl;
        }    
        

        // STEP3 填充数据
        encode(resultCode, request, rspBuf, sRoomKey);
        
        __CATCH__
        FUNC_EXIT("", 0);
        return 0;
    }

private:

    static void encode(int64_t resultCode, CreateTableClubRequest &request, vector<tars::Char> &rspBuf, string sRoomKey)
    {
        CreateTableClubRespons  response;
        response._sRoomKey = sRoomKey;

        // resultData是数组
        std::string json;
        response.toString(json);
        std::string resultData = "[" + json + "]";

        int64_t totalItems = 1;  //总条数
        int64_t totalPages = 1;  //总页数
        GMResponse rsp(resultCode, "", resultData, totalItems, totalPages);
        std::string resultJson;
        rsp.toString(resultJson);
        rspBuf.assign(resultJson.begin(), resultJson.end());
    }
private:
     CString _sRoomKey;     // 房间ID
};