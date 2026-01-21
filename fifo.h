#ifndef __FIFO_INC__
#define __FIFO_INC__

/************************************************
  Simple FIFO Control

  - This is a FIFO (First-In, First-Out) library for general-purpose use.
  - It operates on a user-provided buffer. No dynamic memory allocation is used.
  - The data type (char, short, long) is fixed at initialization for efficient,
    type-safe pointer operations.
*************************************************/

/*******************************
[ function organization - PlantUML ]

@startuml
!theme plain
skinparam packageStyle rectangle
skinparam defaultFontName Arial
skinparam defaultFontSize 10

title fifo.c - Type-Aware FIFO Control

package "FIFO Public API" {
  enum FIFO_ElementType
  class FIFO_initialize
  class FIFO_push
  class FIFO_pop
  class FIFO_is_full
  class FIFO_is_empty
}

package "FIFO Control Block" {
  class "struct FIFO_cb" as FIFO_cb {
    void *pStart
    void *pEnd
    void *pRead
    void *pWrite
    unsigned int count
    unsigned int capacity
    FIFO_ElementType type
  }
}

FIFO_initialize -down-> FIFO_cb : modifies
FIFO_push -down-> FIFO_cb : modifies
FIFO_pop -down-> FIFO_cb : modifies
FIFO_is_full -down-> FIFO_cb : reads
FIFO_is_empty -down-> FIFO_cb : reads

note right of FIFO_initialize
  Initializes the FIFO control block
  with a user-provided buffer and
  a specific element type.
end note

note right of FIFO_push
  Pushes one element onto the FIFO
  using type-safe pointer assignment.
end note
@enduml
*******************************/

/*
 * Defines the data type the FIFO will handle.
 */
enum FIFO_ElementType {
  FIFO_TYPE_CHAR,
  FIFO_TYPE_SHORT,
  FIFO_TYPE_LONG
};

/*
 * FIFO Control Block
 * The user must allocate memory for this structure.
 */
struct FIFO_cb {
  void *pStart;                 /* Pointer to the start of the user-provided buffer */
  void *pEnd;                   /* Pointer to the end (one past the last element) of the buffer */
  void *pRead;                  /* Pointer to the next element to be read */
  void *pWrite;                 /* Pointer to the next position to be written */
  
  unsigned int count;           /* Number of elements currently in the FIFO */
  unsigned int capacity;        /* Maximum number of elements */
  enum FIFO_ElementType type;   /* Type of the elements in the FIFO */
};

/**
 * @brief Initializes a FIFO control block.
 * @param fifo_cb Pointer to the FIFO_cb structure to be initialized.
 * @param buffer Pointer to the buffer to be used by the FIFO.
 * @param capacity The maximum number of elements the buffer can hold.
 * @param type The type of data elements the FIFO will store.
 */
void FIFO_initialize(struct FIFO_cb *fifo_cb, void *buffer, unsigned int capacity, enum FIFO_ElementType type);

/**
 * @brief Pushes an element onto the FIFO.
 * @param fifo_cb Pointer to the FIFO_cb structure.
 * @param element Pointer to the element to be pushed. The data is copied from this address.
 * @return 0 on success, -1 if the FIFO is full.
 */
int FIFO_push(struct FIFO_cb *fifo_cb, const void *element);

/**
 * @brief Pops an element from the FIFO.
 * @param fifo_cb Pointer to the FIFO_cb structure.
 * @param element Pointer to a variable to receive the popped element. The data is copied to this address.
 * @return 0 on success, -1 if the FIFO is empty.
 */
int FIFO_pop(struct FIFO_cb *fifo_cb, void *element);

/**
 * @brief Checks if the FIFO is full.
 * @param fifo_cb Pointer to the FIFO_cb structure.
 * @return 1 if full, 0 otherwise.
 */
int FIFO_is_full(const struct FIFO_cb *fifo_cb);

/**
 * @brief Checks if the FIFO is empty.
 * @param fifo_cb Pointer to the FIFO_cb structure.
 * @return 1 if empty, 0 otherwise.
 */
int FIFO_is_empty(const struct FIFO_cb *fifo_cb);

#endif /* __FIFO_INC__ */
