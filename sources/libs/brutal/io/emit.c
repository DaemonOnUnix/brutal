#include <brutal/base/attributes.h>
#include <brutal/debug/assert.h>
#include <brutal/io/emit.h>

void emit_init(Emit *self, IoWriter *writer)
{
    self->indent = 0;
    self->indent_size = 4;
    self->writer = writer;
    self->line_begin = true;
}

void emit_deinit(Emit *self)
{
    *self = (Emit){};
}

void emit_ident_size(Emit *self, int size)
{
    self->indent_size = size;
}

void emit_ident(Emit *self)
{
    self->indent++;
}

void emit_deident(Emit *self)
{
    assert_greater_equal(self->indent, 0);
    self->indent--;
}

static IoResult emit_write_impl(Emit *self, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    size_t written = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (self->line_begin)
        {
            for (int j = 0; j < self->indent_size * self->indent; j++)
            {
                written += TRY(IoResult, io_put(self->writer, ' '));
            }

            self->line_begin = false;
        }

        if (data[i] == '\n')
        {
            self->line_begin = true;
        }

        written += TRY(IoResult, io_put(self->writer, data[i]));
    }

    return OK(IoResult, written);
}

IoWriter emit_writer(Emit *self)
{
    return (IoWriter){
        .write = (IoWrite *)emit_write_impl,
        .context = self,
    };
}
