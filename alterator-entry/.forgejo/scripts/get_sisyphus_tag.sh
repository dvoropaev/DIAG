#!/bin/bash

make_request() {
    local url=$1
    local attempt=0
    local max_attempts=10
    local response
    local http_code

    while [ $attempt -lt $max_attempts ]; do
        response=$(curl -s -w "%{http_code}" "$url")
        http_code=${response: -3}
        response_body=${response%???}
        
        if [ "$http_code" -ne 429 ]; then
            if [ "$http_code" -eq 200 ]; then
                VERSION=$(echo "$response_body" | jq -r '.packages[0].version')
                RELEASE=$(echo "$response_body" | jq -r '.packages[0].release')
                echo "$http_code ${VERSION}-${RELEASE}"

                return 0
            fi

            echo "$http_code null"
            return 1
        fi
        
        attempt=$((attempt + 1))
        sleep 5
    done

    echo "$http_code null"
    return 1
}

get_sisyphus_version() {
    local package_name="$1"
    local URL="https://rdb.altlinux.org/api/package/package_info?name=${package_name}&source=false&branch=sisyphus&full=false"
    local http_code
    local tag

    read http_code tag < <(make_request "$URL")

    if [ $http_code -eq 429 ]; then
        echo "❌ Maximum retry attempts reached for rate limiting"
        return 1
    fi

    if [ $http_code -eq 404 ]; then
        echo "🔄 Package version for x86_64 not found. Trying noarch version..."
        read http_code tag < <(make_request "${URL}&arch=noarch")

        if [ $http_code -eq 429 ]; then
            echo "❌ Maximum retry attempts reached for rate limiting"
            return 1
        fi
    fi

    if [ $http_code -ne 200 ]; then
        echo "❌ Failed to retrieve $package_name package information in Sisyphus (HTTP $http_code)"
        return 1
    fi

    SISYPHUS_TAG="$tag"
    echo "✅ Latest Sisyphus tag for ${package_name}: $SISYPHUS_TAG"

    export SISYPHUS_TAG
    return 0
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    get_sisyphus_version "$1"
fi
