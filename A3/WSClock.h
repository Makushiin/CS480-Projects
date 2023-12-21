
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#ifndef WSCLOCK_PAGE_REPLACEMENT_H
#define WSCLOCK_PAGE_REPLACEMENT_H

#include <vector>

struct PageFrame {
    int vpn;           // VPN (Virtual Page Number) in the frame
    int lastAccess;    // Last access time
    bool dirty;        // Dirty flag
};

class WSClock {
public:
    WSClock(int numFrames, int ageThreshold);
    int performPageReplacement(int newVpn);
    void updateAccessTime(int frameIndex, int accessTime);
private:
    int numFrames_;
    int ageThreshold_;
    int currentFrame_;
    std::vector<PageFrame> circularList_;
};

#endif
