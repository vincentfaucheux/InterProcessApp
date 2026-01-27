#include <pybind11/pybind11.h>
#include<atomic>
#include "../PipeComSo/PipeComSo.h"       // depuis PipeComSo
#include "../EndPointComSo/EndPointComSo.h"   // depuis EndPointComSo

namespace py = pybind11;

class PyEndPointCom {
public:
    PyEndPointCom(
        const std::string& app_path,
        const std::string& pipe_write,
        const std::string& pipe_read,
        py::function on_write_created = py::none()
    )
        : cb_write_created(on_write_created)
    {
        all_ok = -2;
        bWriteCreated.store(false, std::memory_order_release);
        bDataReceived.store(false, std::memory_order_release);

        ep = new tEndPointCom(
            app_path,
            pipe_write,
            pipe_read,
            &all_ok,
            &PyEndPointCom::write_created_trampoline,
            this
        );

        if(all_ok > -1)
        {
            bWriteCreated.store(true, std::memory_order_release);
        }
    }

    ~PyEndPointCom()
    {
        delete ep;
    }

    bool write(py::bytes data)
    {
        std::string buffer = data;
        return ep->WriteData(
            reinterpret_cast<const uint8_t*>(buffer.data()),
            buffer.size()
        );
    }

    py::bytes read()
    {
        bDataReceived.store(false, std::memory_order_release);
        std::vector<uint8_t> out;
        if (!ep->ReadData(&out))
            return py::bytes();

        return py::bytes(
            reinterpret_cast<const char*>(out.data()),
            out.size()
        );
    }


    void set_callback(py::function cb)
    {
        cb_data_received = cb;

        ep->SetCbReceivedData(&PyEndPointCom::data_received_trampoline, this);
    }

    void poolWriteCreated()
    {
        if (bWriteCreated.exchange(false))
        {
            if (cb_write_created)
            {
                py::gil_scoped_acquire gil;
                if (cb_write_created)
                {
                    cb_write_created();
                }
            }
        }
    }

    void poolDataReceived()
    {
        if (bDataReceived.exchange(false))
        {
            if (cb_data_received)
            {
                py::gil_scoped_acquire gil;
                if (cb_data_received)
                {
                    cb_data_received();
                }
            }
        }
    }

private:

    static void write_created_trampoline(void* ctx)
    {
        auto* self = static_cast<PyEndPointCom*>(ctx);
        if (!self->cb_write_created)
            return;

        self->bWriteCreated.store(true, std::memory_order_release);
    }

    static void data_received_trampoline(void* ctx)
    {
        auto* self = static_cast<PyEndPointCom*>(ctx);

        self->bDataReceived.store(true, std::memory_order_release);}

    tEndPointCom* ep = nullptr;
    int all_ok = -2;
    std::atomic<bool> bWriteCreated{ false };
    std::atomic<bool> bDataReceived{ false };
    py::function cb_write_created;
    py::function cb_data_received;

};

PYBIND11_MODULE(pipecom, m)
{
    m.doc() = "Python bindings for PipeCom";

    py::class_<PyEndPointCom>(m, "EndPointCom")
    .def(py::init<
        const std::string&,
        const std::string&,
        const std::string&,
        py::function
    >())
    .def("write", &PyEndPointCom::write)
    .def("read", &PyEndPointCom::read)
    .def("set_callback", &PyEndPointCom::set_callback)
    .def("pool_write_created", &PyEndPointCom::poolWriteCreated)
    .def("pool_data_received", &PyEndPointCom::poolDataReceived);
}
