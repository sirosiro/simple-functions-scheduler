#ifndef __FIFO_INC__
#define __FIFO_INC__

/******************************************************************************
 * @file fifo.h
 * @brief A type-aware, memory-efficient FIFO (First-In, First-Out) library.
 *
 * @responsibility
 * Provides a fast and memory-efficient queuing mechanism for fixed-type data
 * elements (char, short, long), intended for inter-task communication.
 *
 * @implementation_notes
 * This FIFO is implemented with type-specific pointer manipulation for performance.
 * The read/write heads are managed by direct pointers to the buffer. For the
 * detailed design rationale behind this approach, please refer to the
 * "Key Architectural Decisions" section in ARCHITECTURE_MANIFEST.md.
 *
 * @preconditions
 * The user is responsible for allocating both the `FIFO_cb` control block and
 * the data buffer itself. This library performs no dynamic memory allocation,
 * adhering to Core Principle 2.
 *****************************************************************************/

/*******************************
[ function organization - PlantUML ]
(diagram omitted for brevity)
*******************************/

/**
 * @enum FIFO_ElementType
 * @brief Defines the data type the FIFO will handle.
 */
enum FIFO_ElementType {
  FIFO_TYPE_CHAR,
  FIFO_TYPE_SHORT,
  FIFO_TYPE_LONG
};

/**
 * @struct FIFO_cb
 * @brief The control block for a FIFO instance.
 * @note The user must allocate memory for this structure before use.
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
 * @brief Initializes a FIFO control block.
 * @note This must be called before any other FIFO operation on the instance.
 * @param fifo_cb Pointer to the user-allocated `FIFO_cb` structure. Must not be NULL.
 * @param buffer Pointer to the user-allocated data buffer, sized to hold `capacity` elements of `type`. Must not be NULL.
 * @param capacity The maximum number of elements the buffer can hold.
 * @param type The data type the FIFO will handle (e.g., FIFO_TYPE_CHAR).
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
