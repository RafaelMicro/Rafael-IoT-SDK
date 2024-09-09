#include "ncp_cpc.hpp"

#include <stdio.h>

#include <openthread/ncp.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/misc.h>

#include "openthread-core-config.h"
#include "openthread-system.h" // for otSysEventSignalPending()
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/new.hpp"

// #include "log.h"

#ifdef CONFIG_OT_RCP_EZMESH

extern void otSysEventSignalPending(void);
namespace ot {
namespace Ncp {

extern "C" {
    extern void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);
}

#if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK == 0

static OT_DEFINE_ALIGNED_VAR(sNcpRaw, sizeof(NcpCPC), uint64_t);

extern "C" void otAppNcpInit(otInstance *aInstance)
{
    NcpCPC *  ncpCPC   = nullptr;
    Instance *instance = static_cast<Instance *>(aInstance);

    ncpCPC = new (&sNcpRaw) NcpCPC(instance);

    if (ncpCPC == nullptr || ncpCPC != NcpBase::GetNcpInstance())
    {
        OT_ASSERT(false);
    }
}

#endif // OPENTHREAD_ENABLE_NCP_VENDOR_HOOK == 0

NcpCPC::NcpCPC(Instance *aInstance)
    : NcpBase(aInstance)
    , mIsReady(false)
    , mIsWriting(false)
    , mCpcSendTask(*aInstance, SendToCPC)
    , mCpcEndpointErrorTask(*aInstance, HandleEndpointError)
    , mCpcOpenEndpointTask(*aInstance, HandleOpenEndpoint)
{
}

void NcpCPC::HandleOpenEndpoint(Tasklet &aTasklet)
{
    OT_UNUSED_VARIABLE(aTasklet);
    static_cast<NcpCPC *>(GetNcpInstance())->HandleOpenEndpoint();
}

void NcpCPC::HandleOpenEndpoint(void)
{

    status_t status = cpc_open_service_endpoint(&mUserEp, CPC_ENDPOINT_15_4, 0, 10);

    if (status == CPC_STATUS_ALREADY_EXISTS)
    {
        return;
    }
    else if (status == CPC_STATUS_BUSY)
    {
        static_cast<NcpCPC *>(GetNcpInstance())->mCpcOpenEndpointTask.Post();
        return;
    }    

    OT_ASSERT(status == CPC_STATUS_OK);

    log_info("Opened Thread RCP EP");

    status = cpc_set_endpoint_option(&mUserEp, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                                        reinterpret_cast<void *>(HandleCPCSendDone));

    OT_ASSERT(status == CPC_STATUS_OK);

    status = cpc_set_endpoint_option(&mUserEp, CPC_ENDPOINT_ON_IFRAME_RECEIVE,
                                        reinterpret_cast<void *>(HandleCPCReceive));

    OT_ASSERT(status == CPC_STATUS_OK);

    status = cpc_set_endpoint_option(&mUserEp, CPC_ENDPOINT_ON_ERROR,
                                        reinterpret_cast<void *>(HandleCPCEndpointError));

    OT_ASSERT(status == CPC_STATUS_OK);

    mTxFrameBuffer.SetFrameAddedCallback(HandleFrameAddedToNcpBuffer, this);
}

void NcpCPC::HandleFrameAddedToNcpBuffer(void *                   aContext,
                                         Spinel::Buffer::FrameTag aTag,
                                         Spinel::Buffer::Priority aPriority,
                                         Spinel::Buffer *         aBuffer)
{
    OT_UNUSED_VARIABLE(aBuffer);
    OT_UNUSED_VARIABLE(aTag);
    OT_UNUSED_VARIABLE(aPriority);

    static_cast<NcpCPC *>(aContext)->HandleFrameAddedToNcpBuffer();
}

void NcpCPC::HandleFrameAddedToNcpBuffer(void)
{
    if (mIsReady && !mIsWriting)
        mCpcSendTask.Post();
}

void NcpCPC::SendToCPC(Tasklet &aTasklet)
{
    OT_UNUSED_VARIABLE(aTasklet);
    static_cast<NcpCPC *>(GetNcpInstance())->SendToCPC();
}

// may need to be updated to handle sleepy devices. Refer to NcpUart::EncodeAndSendToUart
void NcpCPC::SendToCPC(void)
{
    Spinel::Buffer &txFrameBuffer = mTxFrameBuffer;
    uint16_t        bufferLen;
    status_t        status;

    VerifyOrExit(mIsReady && !mIsWriting && !txFrameBuffer.IsEmpty());

    mIsWriting = true;
    IgnoreError(txFrameBuffer.OutFrameBegin());
    bufferLen = txFrameBuffer.OutFrameGetLength();

    txFrameBuffer.OutFrameRead(bufferLen, mCpcTxBuffer);
    status = cpc_write(&mUserEp, mCpcTxBuffer, bufferLen, 0, NULL);
    if(status != CPC_STATUS_OK)
    {
        log_error("EP 15.4 Write CPC fail %04X", status);
        static_cast<NcpCPC *>(GetNcpInstance())->HandleSendDone();
        cpc_system_reset(0);
    }
    IgnoreError(txFrameBuffer.OutFrameRemove());

exit:
    // If the CPCd link isn't ready yet, just remove the frame from
    // the queue so that it doesn't fill up unnecessarily
    if (!mIsReady)
    {
        IgnoreError(txFrameBuffer.OutFrameRemove());
    }

    return;
}

void NcpCPC::HandleCPCSendDone(cpc_user_endpoint_id_t endpoint_id, void *buffer, void *arg, status_t status)
{
    OT_UNUSED_VARIABLE(endpoint_id);
    OT_UNUSED_VARIABLE(buffer);
    OT_UNUSED_VARIABLE(arg);
    OT_UNUSED_VARIABLE(status);
    static_cast<NcpCPC *>(GetNcpInstance())->HandleSendDone();
}

void NcpCPC::HandleSendDone(void)
{
    mIsWriting = false;
    memset(mCpcTxBuffer, 0, sizeof(mCpcTxBuffer));

    if (!mTxFrameBuffer.IsEmpty())
        mCpcSendTask.Post();
}

void NcpCPC::HandleCPCReceive(cpc_user_endpoint_id_t endpoint_id, void *arg)
{
    OT_UNUSED_VARIABLE(endpoint_id);
    OT_UNUSED_VARIABLE(arg);

    otSysEventSignalPending(); // wakeup ot task
}

void NcpCPC::HandleCPCEndpointError(uint8_t endpoint_id, void *arg)
{
    OT_UNUSED_VARIABLE(endpoint_id);
    OT_UNUSED_VARIABLE(arg);

    // Can't close and open endpoints in this context
    static_cast<NcpCPC *>(GetNcpInstance())->mCpcEndpointErrorTask.Post();
}

void NcpCPC::HandleEndpointError(Tasklet &aTasklet)
{
    OT_UNUSED_VARIABLE(aTasklet);
    static_cast<NcpCPC *>(GetNcpInstance())->HandleEndpointError();
}

void NcpCPC::HandleEndpointError(void)
{
    uint8_t ep_state;
    ep_state = cpc_get_endpoint_state(&mUserEp);

    log_warn("EP 15.4 Error %d!", ep_state);
    if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
        cpc_system_reset(0);

    //OT_ASSERT(cpc_close_endpoint(&mUserEp) == CPC_STATUS_OK);

    cpc_close_endpoint(&mUserEp);
    cpc_set_state(&mUserEp, CPC_STATE_OPEN);

    mUserEp.ref_count = 1u;
    mIsReady = false;
}

extern "C" void rf_ot_cpc_rcp_process(void)
{
    NcpCPC *ncpCPC = static_cast<NcpCPC *>(NcpBase::GetNcpInstance());

    if (ncpCPC != nullptr)
    {
        ncpCPC->ProcessCpc();
    }
}
extern "C" void rf_ot_cpc_init(void)
{
    NcpCPC *ncpCPC = static_cast<NcpCPC *>(NcpBase::GetNcpInstance());

    if (ncpCPC != nullptr)
    {
        ncpCPC->HandleOpenEndpoint();
    }
}
void NcpCPC::ProcessCpc(void)
{
    status_t status;
    void *      data;
    uint16_t    dataLength;
    //info("ProcessCpc\n");
    HandleOpenEndpoint();

    status = cpc_read(&mUserEp, &data, &dataLength, 0,
                         CPC_FLAG_NO_BLOCK); // In bare-metal read is always
                                                // non-blocking, but with rtos
                                                // since this function is called
                                                // in the cpc task, it must not
                                                // block.
    SuccessOrExit(status);

    if (!mIsReady)
    {
        mIsReady = true;
    }
    super_t::HandleReceive(static_cast<uint8_t *>(data), dataLength);
    status = cpc_free_rx_buffer(data);
    OT_ASSERT(status == CPC_STATUS_OK);

exit:
    if (mIsReady && !mTxFrameBuffer.IsEmpty())
        mCpcSendTask.Post();
}

} // namespace Ncp
} // namespace ot

#endif // OPENTHREAD_CONFIG_NCP_CPC_ENABLE
