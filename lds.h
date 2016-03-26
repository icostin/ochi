
/*  lds_status_t  */
/**
 *  Status for linear data space functions.
 */
typedef enum lds_status_e lds_status_t;

/*  lds_t  */
/**
 *  Linear data space.
 */
typedef struct lds_s lds_t;

typedef struct lds_change_class_s lds_change_class_t;

/*  lds_change_t  */
/**
 *  Linear data space change.
 *  Changes are kept in a stack to allow undo-ing them.
 *  They contain the old data and the new data which may have different sizes.
 */
typedef struct lds_change_s lds_change_t;

typedef struct lds_replace_s lds_replace_t;

/*  ldb_t  */
/**
 *  Linear data block.
 */
typedef struct ldb_s ldb_t;

/*  ldb_class_t  */
/**
 *  Linear data block class.
 */
typedef struct ldb_class_s ldb_class_t;

/*  ldb_file_chunk_t  */
/**
 *  Linear data block - file chunk.
 */
typedef struct ldb_file_chunk_s ldb_file_chunk_t;

enum lds_status_e
{
    LDS_OK = 0,
};

struct lds_change_class_s
{
    unsigned int (ZLX_CALL * undo) (lds_t * lds, lds_change_t * chg);
    size_t size;
    char const * name;
};

struct lds_change_s
{
    zlx_np_t stack; // change stack; next goes to a newer change
    lds_change_class_t * cls;
};

struct lds_replace_s
{
    lds_change_t chg;
    uint8_t * od_a;
    uint8_t * nd_a;
    size_t od_n, od_m;
    size_t nd_n, nd_m;
};

struct ldb_class_s
{
    lds_status_t (ZLX_CALL * get_cached) 
        (
            ldb_t * ldb,
            uint64_t offset,
            size_t size,
            void * out_data,
            size_t * out_size
        );
    lds_status_t (ZLX_CALL * get)
        (
            ldb_t * ldb,
            uint64_t offset,
            size_t size,
            void * out_data,
            size_t * out_size
        );
    size_t size;
    char const * name;
};

struct ldb_s
{
    ldb_class_t * cls;
    lds_t * lds;
    ldb_t * rbt_links[2];
    uint64_t base;
    uint64_t size;
    uint8_t rbt_color;
};

struct ldb_file_chunk_s
{
    ldb_t ldb;
    uint64_t offset;
};

struct lds_s
{
    ldb_t rbt_guard;
    uint64_t size;
};

struct lds_file_s
{
    lds_t lds;
    zlx_file_t * zf;

};



