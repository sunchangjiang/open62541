/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#ifdef UA_ENABLE_AMALGAMATION
#include "open62541.h"
#else
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#endif

#include "open62541/namespace_di_generated.h"
#include "open62541/namespace_powerlink_generated.h"

#include <signal.h>
#include <stdlib.h>

UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);

    /* create nodes from nodeset */
    UA_StatusCode retval = namespace_di_generated(server);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Adding the DI namespace failed. Please check previous error output.");
        UA_Server_delete(server);
        UA_ServerConfig_delete(config);
        return EXIT_FAILURE;
    }
    retval |= namespace_powerlink_generated(server);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Adding the Powerlink namespace failed. Please check previous error output.");
        UA_Server_delete(server);
        UA_ServerConfig_delete(config);
        return EXIT_FAILURE;
    }

    retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
