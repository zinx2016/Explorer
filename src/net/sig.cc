#include "sig.h"

namespace Explorer {

std::unordered_map<int, std::function<void()>> Signal::handlers_;

} // namespace Explorer;
