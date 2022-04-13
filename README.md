# syncpp
At the moment , only implemented:
1) `storing_mutex` - wrapper over `std::mutex` that explicitly shows which resource mutex is protecting
2) `rw_lock` - wrapper over `std::shared_mutex` that explicitly shows which resource shared mutex is protecting
3) `channel` and it's specializaton for `BufferSize == 1` (WIP!)
