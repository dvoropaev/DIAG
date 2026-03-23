setup() {
    tmpdir="$(mktemp -d)"
}

@test "Check if report is readable" {
    ./diag-domain-client --report | tar --extract --gzip -C "$tmpdir"
}

teardown() {
    rm -rf "$tmpdir"
}
