#ifndef __FIFO_INC__
#define __FIFO_INC__

/******************************************************************************
 * @file fifo.h
 * @brief A type-aware, memory-efficient FIFO (First-In, First-Out) library.
 *
 * @responsibility
 * To provide a safe and fast queuing mechanism for fixed-type data elements
 * (like events or messages) for inter-task communication. It ensures that
 * the user-provided buffer is used efficiently without wasted space, as per
 * Core Principle 2 (Static Memory Management).
 *
 * @rationale
 * This FIFO is implemented with type-specific pointer manipulation instead of
 * a generic `memcpy`. This design was chosen for performance and type safety,
 * as it avoids the overhead of byte-by-byte copying for known basic types
 * (char, short, long). It is intentionally distinct from a "ring buffer"
 * which is intended for continuous data streams. The read/write heads are
 * managed by direct pointers rather than indices to further optimize access.
 * This decision is logged in ARCHITECTURE_MANIFEST.md.
 *
 * @preconditions
 * The user must provide a valid memory buffer appropriately sized for the
 * specified capacity and element type. The control block structure (`FIFO_cb`)
 * must also be allocated by the user.
 *
 * @prohibitions
 * This implementation is optimized for basic integer types. It is not intended
 * for handling complex structs, pointers, or data requiring deep copies.
 *****************************************************************************/

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

/**
 * @enum FIFO_ElementType
 * @brief Defines the data type the FIFO will handle.
 * @rationale Using an enum allows for type-safe, optimized operations
 *            within the FIFO implementation.
 */
enum FIFO_ElementType {
  FIFO_TYPE_CHAR,
  FIFO_TYPE_SHORT,
  FIFO_TYPE_LONG
};

/**
 * @struct FIFO_cb
 * @brief The control block for a FIFO instance.
 * @preconditions The user must allocate memory for this structure.
 */
struct FIFO_cb {
  void *pStart;                 /**< Pointer to the start of the user-provided buffer. */
  void *pEnd;                   /**< Pointer to the end (one past the last valid address) of the buffer. */
  void *pRead;                  /**< Pointer to the next element to be read. */
  void *pWrite;                 /**< Pointer to the next position to be written. */
  
  unsigned int count;           /**< Number of elements currently in the FIFO. */
  unsigned int capacity;        /**< Maximum number of elements the FIFO can hold. */
  enum FIFO_ElementType type;   /**< The data type of the elements in the FIFO. */
};

/**
 * @brief Initializes a FIFO control block. This must be called before any other
 *        FIFO operation.
 * @param fifo_cb Pointer to the user-allocated `FIFO_cb` structure. Must not be NULL.
 * @param buffer Pointer to the user-allocated data buffer. Must not be NULL.
 * @param capacity The maximum number of elements the buffer can hold.
 * @param type The data type the FIFO will handle (e.g., FIFO_TYPE_CHAR).
 * @preconditions
 * - `fifo_cb` and `buffer` must point to valid, allocated memory.
 * - The size of the `buffer` must be at least `capacity * sizeof(type)`.
 */
void FIFO_initialize(struct FIFO_cb *fifo_cb, void *buffer, unsigned int capacity, enum FIFO_ElementType type);

/**
 * @brief Pushes one element onto the FIFO.
 * @param fifo_cb Pointer to the initialized `FIFO_cb` structure.
 * @param element Pointer to the element to be pushed. The data is copied from this address.
 * @return 0 on success, -1 if the FIFO is full or if parameters are invalid.
 */
int FIFO_push(struct FIFO_cb *fifo_cb, const void *element);

/**
 * @brief Pops one element from the FIFO.
 * @param fifo_cb Pointer to the initialized `FIFO_cb` structure.
 * @param element Pointer to a variable that will receive the popped element. The data is copied to this address.
 * @return 0 on success, -1 if the FIFO is empty or if parameters are invalid.
 */
int FIFO_pop(struct FIFO_cb *fifo_cb, void *element);

/**
 * @brief Checks if the FIFO is full.
 * @param fifo_cb Pointer to the initialized `FIFO_cb` structure.
 * @return 1 if the FIFO is full, 0 otherwise.
 */
int FIFO_is_full(const struct FIFO_cb *fifo_cb);

/**
 * @brief Checks if the FIFO is empty.
 * @param fifo_cb Pointer to the initialized `FIFO_cb` structure.
 * @return 1 if the FIFO is empty, 0 otherwise.
 */
int FIFO_is_empty(const struct FIFO_cb *fifo_cb);

#endif /* __FIFO_INC__ */
