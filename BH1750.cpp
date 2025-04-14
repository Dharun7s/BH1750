#include "BH1750.h"

BH1750::BH1750(uint8_t addr) : _address(addr), _mode(UNCONFIGURED), _initialized(false) {
}

bool BH1750::begin(Mode mode) {
    Wire.begin();
    _initialized = configure(mode);
    return _initialized;
}

bool BH1750::configure(Mode mode) {
    if (mode == UNCONFIGURED) {
        return false;
    }

    Wire.beginTransmission(_address);
    Wire.write(static_cast<uint8_t>(mode));
    if (Wire.endTransmission() == 0) {
        _mode = mode;
        return true;
    }
    return false;
}

float BH1750::readLightLevel() {
    if (!_initialized) {
        return -1.0f;
    }


    if (_mode == ONE_TIME_HIGH_RES_MODE || 
        _mode == ONE_TIME_HIGH_RES_MODE_2 || 
        _mode == ONE_TIME_LOW_RES_MODE) {
        if (!configure(_mode)) {
            return -1.0f;
        }
    }


    unsigned long measurementDelay = 0;
    switch (_mode) {
        case CONTINUOUS_HIGH_RES_MODE:
        case ONE_TIME_HIGH_RES_MODE:
        case CONTINUOUS_HIGH_RES_MODE_2:
        case ONE_TIME_HIGH_RES_MODE_2:
            measurementDelay = 180;
            break;
        case CONTINUOUS_LOW_RES_MODE:
        case ONE_TIME_LOW_RES_MODE:
            measurementDelay = 24;
            break;
        default:
            return -1.0f;
    }

    delay(measurementDelay);

    if (Wire.requestFrom(_address, static_cast<uint8_t>(2)) != 2) {
        return -1.0f; // Not enough data received
    }

    uint16_t level = Wire.read() << 8;
    level |= Wire.read();

  
    float lux = level / 1.2f; 
    
    // For mode 2 (different measurement range)
    if (_mode == CONTINUOUS_HIGH_RES_MODE_2 || _mode == ONE_TIME_HIGH_RES_MODE_2) {
        lux = level / 2.4f;
    }

    return lux;
}
