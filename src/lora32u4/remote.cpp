#include "Remote.h"

RemoteClass::RemoteClass() :
    _changed(true),
    _incomming_updated(false),
    _incomming_timestamp(0),
    _outgoing_timestamp(0),
    _valid(false)
{
    _outgoing.sender = 'T';
    _outgoing.msg_id = 0;
}

bool RemoteClass::isValid() {
    // if not updated, return false
    if (! _valid)
        return false;
    // check age, if not too old return true. if too old, set valid to false
    unsigned long now = millis();
    if (now -_incomming_timestamp < REMOTE_EXPIRES)
        return true;
    else
        return _valid = false;
}

bool RemoteClass::transmit(bool force) {
    unsigned long now = millis();
    if (now - _outgoing_timestamp >= REMOTE_INTERVAL || force || _changed) {
        _writePacket();
        _changed = false;
        _outgoing_timestamp = now;
        return true;
    } else {
        return false;
    }
}
void RemoteClass::setThruster(Move state) {
    if (_outgoing.thruster != state) {
        _changed = true;
        _outgoing.thruster = state;
    }
}

Move RemoteClass::getThruster() {
    return isValid() ? _incomming.thruster : UNKNOWN;
}

void RemoteClass::setWindlass(Move state) {
    if (_outgoing.windlass != state) {
        _changed = true;
        _outgoing.windlass = state;
    }
}
Move RemoteClass::getWindlass() {
    return isValid() ? _incomming.windlass : UNKNOWN;
}

void RemoteClass::setDepth(float depth) {
    if (_outgoing.depth != depth) {
        _changed = true;
        _outgoing.depth = depth ;
    }
}

float RemoteClass::getDepth() {
    return isValid() ? _incomming.depth : NAN;
}