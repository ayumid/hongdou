cmake_minimum_required(VERSION 3.9)
project(mqtts_onejson_soc)

# 设置CXX编译选项
set(WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ./
    common
    onenet/platforms/include
    onenet/tm
    onenet/utils
    onenet/protocols/mqtt/
    onenet/protocols/mqtt/paho-mqtt
    onenet/protocols/http
    tools/cJSON
    tools/wolfssl/port
    tools/wolfssl/wolfssl-3.15.3
    services/ota
    onenet/security/tls
)

file(GLOB src
    common/log.c
    common/slist.c
    common/utils.c
    onenet/security/tls/tls.c
    onenet/tm/tm_api.c
    onenet/tm/tm_data.c
    onenet/tm/tm_onejson.c
    onenet/tm/tm_mqtt.c
    onenet/tm/tm_subdev.c
    onenet/tm/dev_discov.c
    onenet/utils/dev_token.c
    onenet/platforms/linux/osl_linux.c
    onenet/platforms/linux/tcp_linux.c
    onenet/platforms/linux/time_linux.c
    onenet/platforms/linux/udp_linux.c
    onenet/protocols/mqtt/paho-mqtt/MQTTConnectClient.c
    onenet/protocols/mqtt/paho-mqtt/MQTTSubscribeClient.c
    onenet/protocols/mqtt/paho-mqtt/MQTTConnectServer.c
    onenet/protocols/mqtt/paho-mqtt/MQTTSubscribeServer.c
    onenet/protocols/mqtt/paho-mqtt/MQTTDeserializePublish.c
    onenet/protocols/mqtt/paho-mqtt/MQTTUnsubscribeClient.c
    onenet/protocols/mqtt/paho-mqtt/MQTTFormat.c
    onenet/protocols/mqtt/paho-mqtt/MQTTUnsubscribeServer.c
    onenet/protocols/mqtt/paho-mqtt/MQTTPacket.c
    onenet/protocols/mqtt/paho-mqtt/mqtt_client.c
    onenet/protocols/mqtt/paho-mqtt/MQTTSerializePublish.c
    onenet/protocols/http/http_api.c
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
    examples/things_model/main.c
    examples/things_model/tm_user.c
)
add_definitions(
    -g
    -DCONFIG_ACCESS_LIFE_TIME=60
    -DCONFIG_SEND_BUF_LEN_MAX=1024
    -DCONFIG_RECV_BUF_LEN_MAX=1024
    -DCONFIG_TM_MQTT
	-DCONFIG_PLAT_ARCH_64BIT=1
    -DCONFIG_TM=1
    -DCONFIG_TM_GATEWAY=1
    -DCONFIG_TM_VERSION="1.0"
    -DCONFIG_DEFAULT_REQUEST_TIMEOUT=1000
    -DCONFIG_PACKET_PAYLOAD_LEN_MAX=2048
    -DCONFIG_HAVE_STDINT=1
    -DCONFIG_NETWORK_TLS=1
)

add_executable(${CMAKE_PROJECT_NAME} ${src})
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)
