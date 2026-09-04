#include <algorithm>
#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>

struct CompactionDetails{
    int count{};
};
using ByteView= std::span<std::byte>;

template <typename T>
CompactionDetails Compact( std::vector<T*>& objects, ByteView region){
    if (std::ranges::any_of(objects, [](const T* obj){return !obj;})){
        throw std::logic_error("Nullptr Present");
    }
    if (region.empty()){
        throw std::logic_error("Underlying memory not present");
    }

    if (region.size()< sizeof(T)*objects.size()){
        throw std::logic_error("Memory not big enough");
    }
    std::ranges::sort(objects);

    CompactionDetails details;
    auto GetAdress= [](ByteView location){return location.data();};

    for (const auto*& object: objects){
        const auto tSize= sizeof(T);
        if (const auto asBytes= reinterpret_cast<std::byte*>(objects); asBytes!= GetAdress(region)){
            // move
            T* moved= std::construct_at(reinterpret_cast<T*>(GetAdress(region)), std::move(*objects));
            //destroy prev
            std::destroy_at(object);
            object= moved;
            details.count++;
        }
        if (region.size() <tSize){
            return details;
        }

        region= region.subspan(tSize);

        if (region.empty()){
            return details;
        }

        return details;
    }
}
