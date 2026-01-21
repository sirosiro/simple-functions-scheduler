#include "fifo.h"

/*
 * @brief A static helper to centralize the logic for determining element size.
 * @rationale This avoids scattering `sizeof` calls throughout the code and
 *            keeps the type-to-size mapping in a single location.
 */
static unsigned int get_element_size(enum FIFO_ElementType type)
{
  switch (type) {
    case FIFO_TYPE_CHAR:
      return sizeof(char);
    case FIFO_TYPE_SHORT:
      return sizeof(short);
    case FIFO_TYPE_LONG:
      return sizeof(long);
    default:
      return 0;
  }
}

void FIFO_initialize(struct FIFO_cb *fifo_cb, void *buffer, unsigned int capacity, enum FIFO_ElementType type)
{
  if (!fifo_cb || !buffer) {
    return;
  }
  
  unsigned int element_size = get_element_size(type);
  
  fifo_cb->pStart = buffer;
  fifo_cb->pEnd = (char*)buffer + (capacity * element_size);
  fifo_cb->pRead = buffer;
  fifo_cb->pWrite = buffer;
  
  fifo_cb->capacity = capacity;
  fifo_cb->count = 0;
  fifo_cb->type = type;
}

int FIFO_push(struct FIFO_cb *fifo_cb, const void *element)
{
  if (!fifo_cb || !element) {
    return -1;
  }
  if (FIFO_is_full(fifo_cb)) {
    return -1; /* FIFO is full */
  }

  /* Copy the element into the buffer using type-aware pointer operations */
  switch (fifo_cb->type) {
    case FIFO_TYPE_CHAR:
      *(char*)fifo_cb->pWrite = *(const char*)element;
      break;
    case FIFO_TYPE_SHORT:
      *(short*)fifo_cb->pWrite = *(const short*)element;
      break;
    case FIFO_TYPE_LONG:
      *(long*)fifo_cb->pWrite = *(const long*)element;
      break;
    default:
      return -1; /* Should not happen */
  }

  /* Advance write pointer */
  fifo_cb->pWrite = (char*)fifo_cb->pWrite + get_element_size(fifo_cb->type);
  if (fifo_cb->pWrite >= fifo_cb->pEnd) {
    fifo_cb->pWrite = fifo_cb->pStart; /* Wrap around */
  }
  
  fifo_cb->count++;

  return 0; /* Success */
}

int FIFO_pop(struct FIFO_cb *fifo_cb, void *element)
{
  if (!fifo_cb || !element) {
    return -1;
  }
  if (FIFO_is_empty(fifo_cb)) {
    return -1; /* FIFO is empty */
  }

  /* Copy the element from the buffer using type-aware pointer operations */
  switch (fifo_cb->type) {
    case FIFO_TYPE_CHAR:
      *(char*)element = *(const char*)fifo_cb->pRead;
      break;
    case FIFO_TYPE_SHORT:
      *(short*)element = *(const short*)fifo_cb->pRead;
      break;
    case FIFO_TYPE_LONG:
      *(long*)element = *(const long*)fifo_cb->pRead;
      break;
    default:
      return -1; /* Should not happen */
  }

  /* Advance read pointer */
  fifo_cb->pRead = (char*)fifo_cb->pRead + get_element_size(fifo_cb->type);
  if (fifo_cb->pRead >= fifo_cb->pEnd) {
    fifo_cb->pRead = fifo_cb->pStart; /* Wrap around */
  }
  
  fifo_cb->count--;

  return 0; /* Success */
}

int FIFO_is_full(const struct FIFO_cb *fifo_cb)
{
  if (!fifo_cb) {
    return 0;
  }
  return fifo_cb->count == fifo_cb->capacity;
}

int FIFO_is_empty(const struct FIFO_cb *fifo_cb)
{
  if (!fifo_cb) {
    return 1;
  }
  return fifo_cb->count == 0;
}
