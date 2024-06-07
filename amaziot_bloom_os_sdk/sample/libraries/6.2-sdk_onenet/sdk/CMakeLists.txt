project(mqtts_stream_soc C ASM)

include(${CMAKE_SOURCE_DIR}/cmake/vars.cmake)

set(TARGET ${CMAKE_PROJECT_NAME})

include_directories(
    .
    common
    onenet/iot
    onenet/iot/mqtts
    onenet/utils
    onenet/protocols/mqtt
    onenet/security/tls
    tools/cJSON
    tools/wolfssl/port
    tools/wolfssl/wolfssl-3.15.3
    onenet/platforms/include
)

file(GLOB src
    common/*.c
    onenet/utils/dev_token.c
    onenet/iot/iot_api.c
    onenet/iot/mqtts/iot_mqtts.c
    onenet/protocols/mqtt/paho-mqtt/MQTTConnectClient.c
    onenet/protocols/mqtt/paho-mqtt/MQTTDeserializePublish.c
    onenet/protocols/mqtt/paho-mqtt/MQTTFormat.c
    onenet/protocols/mqtt/paho-mqtt/MQTTPacket.c
    onenet/protocols/mqtt/paho-mqtt/MQTTSerializePublish.c
    onenet/protocols/mqtt/paho-mqtt/MQTTSubscribeClient.c
    onenet/protocols/mqtt/paho-mqtt/MQTTUnsubscribeClient.c
    onenet/protocols/mqtt/paho-mqtt/mqtt_client.c
    onenet/security/tls/aio_tls.c
    tools/cJSON/cJSON.c
    tools/wolfssl/wolfssl-3.15.3/src/internal.c
    tools/wolfssl/wolfssl-3.15.3/src/keys.c
    tools/wolfssl/wolfssl-3.15.3/src/ssl.c
    tools/wolfssl/wolfssl-3.15.3/src/tls.c
    tools/wolfssl/wolfssl-3.15.3/src/wolfio.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/aes.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/asn.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/coding.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hash.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hmac.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/integer.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/md5.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/misc.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/random.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/rsa.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha256.c
    tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/wc_port.c
    examples/onenet_mqtts/main.c
    onenet/platforms/linux/osl_linux.c
    onenet/platforms/linux/tcp_linux.c
    onenet/platforms/linux/time_linux.c
    onenet/platforms/linux/udp_linux.c
)

add_definitions(
    -g
    -DCONFIG_PLAT_LINUX=0
    -DCONFIG_PLAT_ARCH_64BIT=0
    -DCONFIG_DEBUG_LOG=1
    -DCONFIG_DEBUG_LOG_LEVEL_DEBUG=1
    -DCONFIG_ACCESS_ENABLED=1
    -DCONFIG_IOT=1
    -DCONFIG_IOT_MQTTS=1
    -DCONFIG_IOT_MQTTS_TLS=1
    -DCONFIG_PACKET_PAYLOAD_LEN_MAX=128
    -DCONFIG_SEND_BUF_LEN_MAX=256
    -DCONFIG_RECV_BUF_LEN_MAX=256
    -DCONFIG_ACCESS_TOKEN=1
    -DCONFIG_ACCESS_TOKEN_METHOD_SHA1=1
    -DCONFIG_MQTT=1
    -DNETWORK_TLS=1
    -DCONFIG_NETWORK_TLS_WOLFSSL=1
    -DCONFIG_WOLFSSL_CRYPTO=1
    -DCONFIG_WOLFSSL_TLS=1
    -DCONFIG_BUILD_EXAMPLE=1
    -DCONFIG_HAVE_STDINT=1
    -DCONFIG_CJSON=1
    -DRECV_ECHO
)

add_executable(${TARGET} ${src})
target_link_libraries(${TARGET} pthread)