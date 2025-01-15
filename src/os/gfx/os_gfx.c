// Copyright Frost Gorilla, Inc. All Rights Reserved.

internal OS_Event *os_event_list_push_new(Arena *arena, OS_EventList *events, OS_EventKind event_kind) {
  OS_Event *event = push_array(arena, OS_Event, 1);
  DLL_PushBack(events->first, events->last, event);
  events->count += 1;

  event->event_kind = event_kind;
  return event;
}