#include <vector>
#include "algo_interface.h"

using namespace std;
using namespace ms_dsad;

namespace algo_interface {
void FreqCtlContext::SetAdid(const uint64_t &adid) {
    freq_context_.set_adid(adid);
}
::google::protobuf::uint64 FreqCtlContext::GetAdid() const {
    return freq_context_.adid();
}
void FreqCtlContext::SetFreqency(const uint32_t &freqency) {
    freq_context_.set_freqency(freqency);
}
uint32_t FreqCtlContext::GetFreqency() const {
    return freq_context_.freqency();
}
void FreqCtlContext::SetFreqContext(const FreqContext &freq_ctx) {
    freq_context_ = freq_ctx;
}

void AdContext::AddChosedFreqIds(const vector<uint64_t>& chosed_freq_ids) {

}
void AdContext::GetChosedFreqIds(vector<uint64_t>& chosed_freq_ids) {

}

void AdContext::AddChosedFreqId(const uint64_t& chosed_freq_id) {
    abtest_context_.add_chosed_freq_ids(chosed_freq_id);
}

int32_t AdContext::ChosedFreqIdSize() {
    return abtest_context_.chosed_freq_ids_size();
}

uint64_t AdContext::GetChosedFreqId(const int& pos) {
    return abtest_context_.chosed_freq_ids(pos);
}

void AdContext::AddFreqContexts(const vector<FreqCtlContext>& freq_contexts) {
    size_t index = 0;
    FreqContext *freq_ctx_ptr = NULL;
    //::google::protobuf::uint64 adid;
    uint64_t adid;
    for(; index < freq_contexts.size(); ++index) {
        freq_ctx_ptr = abtest_context_.add_freq_cxt_list();
        adid = (freq_contexts[index]).GetAdid();
        freq_ctx_ptr->set_adid(adid);
        freq_ctx_ptr->set_freqency((freq_contexts[index]).GetFreqency());
    }
}
void AdContext::AddFreqContext(const FreqCtlContext& freq_context) {
    FreqContext *freq_ctx_ptr = NULL;
    freq_ctx_ptr = abtest_context_.add_freq_cxt_list();
    freq_ctx_ptr->set_adid(freq_context.GetAdid());
    freq_ctx_ptr->set_freqency(freq_context.GetFreqency());
}
/*
void AdContext::GetFreqContext(vector<FreqCtlContext>& freq_context)
{
  int index = 0;
  for (index = 0; index < abtest_context_.freq_cxt_list_size(); ++index)
  {
    FreqCtlContext freq_ctx;
    FreqContext freq = *(abtest_context_.freq_cxt_list(index));
    freq_ctx.SetFreqContext(freq);
    freq_context.push_back(freq_ctx);
  }
} */

}
