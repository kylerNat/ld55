#define type_id_table CONCAT(type, _id_table)
struct type_id_table
{
    index_table_entry* index_table; //table from id%n_max_elements to index
    int n_max_elements;
    int next_id;
    type* elements; //actual data
    int n_elements;
};

#define init_type_id_table CONCAT(init_, type_id_table)
type_id_table init_type_id_table(int n_max_elements)
{
    return {
        .index_table = (index_table_entry*) stalloc_clear(n_max_elements*sizeof(index_table_entry)),
        .n_max_elements = n_max_elements,
        .next_id = 1,
        .elements = (type*) stalloc(n_max_elements*sizeof(type)),
        .n_elements = 0,
    };
}

type* create_element(type_id_table* table)
{
    for(int i = 0; i < table->n_max_elements; i++)
    {
        int id = table->next_id++;
        index_table_entry* entry = &table->index_table[id % table->n_max_elements];
        if(entry->id == 0)
        {
            entry->id = id;
            entry->index = table->n_elements++;
            table->elements[entry->index] = {};
            table->elements[entry->index].id = id;
            return &table->elements[entry->index];
        }
    }
    return 0;
}

type* get_element(type_id_table* table, int id)
{
    if(id == 0) return 0;
    index_table_entry entry = table->index_table[id % table->n_max_elements];
    if(entry.id == id)
    {
        return &table->elements[entry.index];
    }
    return 0;
}

void delete_element(type_id_table* table, int id)
{
    index_table_entry* entry = &table->index_table[id % table->n_max_elements];
    if(entry->id == id)
    {
        entry->id = 0;
        table->elements[entry->index] = table->elements[--table->n_elements];
        int moved_id = table->elements[entry->index].id;
        if(table->index_table[moved_id % table->n_max_elements].id == moved_id)
        {
            table->index_table[moved_id % table->n_max_elements].index = entry->index;
        }
    }
}

//TODO: auto resizing

#undef type_id_table
