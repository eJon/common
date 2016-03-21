#include <monitor/server/single_app_writer.h>
#include <monitor/common/log.h>
#include <sharelib/util/file_util.h>
#include <sharelib/util/string_util.h>
using namespace std;
using namespace sharelib;
MONITOR_BS;


void SingleAppWriter::Split(std::string in, std::string& front, std::string& end){
    if(in == "") {
        front = in;
        end = "";
        return ;
    }
    string::size_type pos;
    if((pos = in.find_last_of(".")) == string::npos){
        front = "";
        end = "";
        return ;
    }
    front.assign(in, 0, pos);
    pos++;
    if(pos < in.length()){
        end.assign(in, pos, in.length() - pos);
    }else{
        end = "";
    }
    return;
    
}

int SingleAppWriter::Init(AppConfPtr appConfIn, std::string appnameIn){
    AppWriter::Init(appConfIn, appnameIn);
    std::string currentFile = writeDir + "/" + SNAPSHOT_FILE;
    other.reset(new SingleFileWriter());
    if(other->Init(appConfIn, currentFile) != 0){
        return -1;
    }
    
    currentFile = writeDir + "/" + CUMULATION_FILE;
    cumulation.reset(new SingleFileWriter());
    if(cumulation->Init(appConfIn, currentFile) != 0){
        return -1;
    }
    
    return 0;
    /*
    vector<std::string> files;
    FileUtil::ListLocalDir(writeDir, files, 1);
    uint32_t maxindex = 0;
    uint32_t tmp;
    string front, end;
    for(uint32_t i =0;i < files.size();i++){
        Split(files[i],front, end);
        if(StringUtil::StrToUInt32(end.c_str(), tmp)){
            if(tmp > maxindex) maxindex = tmp;
        }
    }
    nowFileIndex = maxindex;
    
    struct stat st;
    FileUtil::GetFileStatus(currentFile, &st);
    nowFizeSize = st.st_size;
    */
    
}

int SingleAppWriter::Write(MonitorInfoPtr& monitorInfo){
    if(monitorInfo->dataid == CUMULATION_DATAID){
        cumulation->Write(monitorInfo);
    }else{
        other->Write(monitorInfo);
    }
    
    return 0;
}
MONITOR_ES;

