#ifndef CREATE_SERVICE_H
#define CREATE_SERVICE_H

#include "Service.h"

#ifdef USE_LIVING
#include "living/LivingService.h"
#elif USE_ROOM
#include "room/RoomService.h"
#endif

// 機能に応じたServiceを返す
template <class T>
Service<T> *create_service(BlynkProtocol<T> *blynk)
{
#ifdef USE_LIVING
  return new LivingService<T>(blynk);
#elif USE_ROOM
  return new RoomService<T>(blynk);
#endif
}

#endif // CREATE_SERVICE_H