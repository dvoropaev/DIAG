setup() {
    tmpdir="$(mktemp -d)"
}

@test "list exposes all system-level tests" {
    run ./diag-system-report --list
    [ "$status" -eq 0 ]
    [[ "$output" == *"check_system_report_binary"* ]]
    [[ "$output" == *"check_system_data_sources"* ]]
    [[ "$output" == *"check_system_diagnostic_context"* ]]
    [[ "$output" == *"test_report_generation"* ]]
}

@test "report is readable" {
    ./diag-system-report --report | tar --extract --xz -C "$tmpdir"
}

teardown() {
    rm -rf "$tmpdir"
}
