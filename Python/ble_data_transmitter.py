import sys
import logging
import asyncio
import threading

from typing import Any, Union

from bless import (
    BlessServer,
    BlessGATTCharacteristic,
    GATTCharacteristicProperties,
    GATTAttributePermissions,
)

import json
############## & C:/Users/dinhv/anaconda3/envs/python.exe C:\Users\dinhv\Desktop\dti3\DTI_Team-BBTL\Python\ble_data_transmitter.py

# Open the JSON file and load hex
with open('output.json', 'r') as f:
    data = json.load(f)
output_string = data['output_string']

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(name=__name__)

# NOTE: Some systems require different synchronization methods.
trigger: Union[asyncio.Event, threading.Event]
if sys.platform in ["darwin", "win32"]:
    trigger = threading.Event()
else:
    trigger = asyncio.Event()

my_service_uuid = "A07498CA-AD5B-474E-940D-16F1FBE7E8CD"
if output_string != None:
    my_service_uuid = my_service_uuid[:-8] + output_string[1:]
    print(my_service_uuid)
else:
    my_service_uuid = my_service_uuid[:-8] + "ffffff04"

def read_request(characteristic: BlessGATTCharacteristic, **kwargs) -> bytearray:
    logger.debug(f"Reading {characteristic.value}")
    return characteristic.value


async def run(loop):
    trigger.clear()
    # Instantiate the server
    my_service_name = "Test Service"
    server = BlessServer(name=my_service_name, loop=loop)
    server.read_request_func = read_request

    # Add Service
    await server.add_new_service(my_service_uuid)

    # Add a Characteristic to the service
    my_char_uuid = "51FF12BB-3ED8-46E5-B4F9-D64E2FEC021B"
    char_flags = (
        GATTCharacteristicProperties.read
        | GATTCharacteristicProperties.write
        | GATTCharacteristicProperties.indicate
    )
    permissions = GATTAttributePermissions.readable | GATTAttributePermissions.writeable
    await server.add_new_characteristic(
        my_service_uuid, my_char_uuid, char_flags, None, permissions
    )

    logger.debug(server.get_characteristic(my_char_uuid))
    await server.start()
    logger.debug("Advertising")
    if trigger.__module__ == "threading":
        trigger.wait()
    else:
        await trigger.wait()

    await asyncio.sleep(2)
    logger.debug("Updating")
    server.get_characteristic(my_char_uuid)
    server.update_value(my_service_uuid, "51FF12BB-3ED8-46E5-B4F9-D64E2FEC021B")
    await asyncio.sleep(5)
    await server.stop()


loop = asyncio.get_event_loop()
loop.run_until_complete(run(loop))