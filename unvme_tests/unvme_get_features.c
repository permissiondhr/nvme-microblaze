/**
 * Copyright (c) 2015-2016, Micron Technology, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief An example using unvme_cmd (generic command) to get features.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "unvme_tests.h"

#include "../nvme/nvme.h"
#include "../nvme/nvme_nvme.h" // for printing get log page structures

static char* features[] = {
    "",
    "1)  Arbitration:",
    "2)  Power Management:",
    "3)  LBA Range Type:",
    "4)  Temperature Threshold:",
    "5)  Error Recovery:",
    "6)  Volatile Write Cache:",
    "7)  Number of Queues:",
    "8)  Interrupt Coalescing:",
    "9)  Interrupt Vector Config:",
    "10) Write Atomicity:",
    "11) Async Event Config:"
};

/**
 * Main program.
 */
int unvme_get_features()
{
	printf("\r\nunvme_get_features test starting...\r\n\n");

    unvme_device_t dev;
    int ret = unvme_open(&dev);
    if (ret) {
    	printf("error: unvme_open\r\n");
    	return 1;
    }
    u64 bufsz = sizeof(nvme_feature_lba_data_t);
    void* buf = unvme_alloc(&dev, bufsz);
    if (!buf) {
    	printf("error: unvme_alloc\r\n");
    	return 1;
    }

    u32 cdw10_15[6] = { 0 };
    u32 res;
    int fid;
    for (fid = NVME_FEATURE_ARBITRATION; fid <= NVME_FEATURE_ASYNC_EVENT; fid++) {
        cdw10_15[0] = fid;
        int err = unvme_cmd(&dev, -1, NVME_ACMD_GET_FEATURES, NSID, buf, bufsz, cdw10_15, &res);

        if (err) {
            printf("%-30s <feature not supported>\n\r", features[fid]);
        } else if (fid == NVME_FEATURE_ARBITRATION) {
            nvme_feature_arbitration_t* arb = (nvme_feature_arbitration_t*)&res;
            printf("%-30s hpw=%u mpw=%u lpw=%u ab=%u\n\r", features[fid],
                    arb->hpw, arb->mpw, arb->lpw, arb->ab);
        } else if (fid == NVME_FEATURE_POWER_MGMT) {
            nvme_feature_power_mgmt_t* pm = (nvme_feature_power_mgmt_t*)&res;
            printf("%-30s ps=%u\n\r", features[fid], pm->ps);
        } else if (fid == NVME_FEATURE_LBA_RANGE) {
            nvme_feature_lba_range_t* lbart = (nvme_feature_lba_range_t*)&res;
            printf("%-30s num=%u\n\r", features[fid], lbart->num);
        } else if (fid == NVME_FEATURE_TEMP_THRESHOLD) {
            nvme_feature_temp_threshold_t* tt = (nvme_feature_temp_threshold_t*)&res;
            printf("%-30s tmpth=%u\n\r", features[fid], tt->tmpth);
        } else if (fid == NVME_FEATURE_ERROR_RECOVERY) {
            nvme_feature_error_recovery_t* er = (nvme_feature_error_recovery_t*)&res;
            printf("%-30s tler=%u\n\r", features[fid], er->tler);
        } else if (fid == NVME_FEATURE_WRITE_CACHE) {
            nvme_feature_write_cache_t* wc = (nvme_feature_write_cache_t*)&res;
            printf("%-30s wce=%u\n\r", features[fid], wc->wce);
        } else if (fid == NVME_FEATURE_NUM_QUEUES) {
            nvme_feature_num_queues_t* nq = (nvme_feature_num_queues_t*)&res;
            printf("%-30s nsq=%u ncq=%u\n\r", features[fid], nq->nsq, nq->ncq);
        } else if (fid == NVME_FEATURE_INT_COALESCING) {
            nvme_feature_int_coalescing_t* intc = (nvme_feature_int_coalescing_t*)&res;
            printf("%-30s time=%u thr=%u\n\r", features[fid], intc->time, intc->thr);
        } else if (fid == NVME_FEATURE_INT_VECTOR) {
            nvme_feature_int_vector_t* intv = (nvme_feature_int_vector_t*)&res;
            printf("%-30s iv=%u cd=%u\n\r", features[fid], intv->iv, intv->cd);
        } else if (fid == NVME_FEATURE_WRITE_ATOMICITY) {
            nvme_feature_write_atomicity_t* wa = (nvme_feature_write_atomicity_t*)&res;
            printf("%-30s dn=%u\n\r", features[fid], wa->dn);
        } else if (fid == NVME_FEATURE_ASYNC_EVENT) {
            nvme_feature_async_event_t* aec = (nvme_feature_async_event_t*)&res;
            printf("%-30s smart=%u\n\r", features[fid], aec->smart);
        }
    }

    unvme_free(&dev, buf);
    unvme_close(&dev);

    printf("\r\nunvme_get_features test complete\r\n");
    return 0;
}

