#!/bin/bash
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
java -jar -Xmx128m ./JTLogger.jar &
