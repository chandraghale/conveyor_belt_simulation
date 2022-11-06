#include <bits/stdc++.h>
namespace cb_factory {
enum cb_component_list { empty = 0, component_a, component_b, final_product };
std::map<cb_component_list, std::string> component_tostring = {
    {empty, "No Item "},
    {component_a, "Component A"},
    {component_b, "Component B"},
    {final_product, "Product P"}
};
class cb_components {
private:
  cb_component_list _component;

public:
  cb_components() {}
  explicit cb_components(cb_component_list _item) : _component(_item) {}
  static std::vector<cb_component_list> getreq_components();
  bool can_take_item(cb_component_list _item) {
    return _item == component_a || component_b;
  }
  bool can_replace_item(cb_component_list _item) { return _item == empty; }
  bool can_make_product(cb_component_list _worker_item_list[]) {
    bool flag = false;
    for (auto &reqc : cb_components::getreq_components()) {
      if (_worker_item_list[0] != reqc || _worker_item_list[1] != reqc) {
        flag = true;
        continue;
      }
      if (!flag)
        return false;
    }
    return true;
  }
  cb_component_list get_item() const { return _component; }
  const void set_item(cb_component_list curr_item) { _component = curr_item; }
  cb_component_list genrate_random_component() {
    return static_cast<cb_component_list>(rand() % 3);
  }
};
std::vector<cb_component_list> cb_components::getreq_components() {
  return {component_a, component_b};
}
class cb_worker {
  std::shared_ptr<cb_components> _component;
  cb_component_list _worker_item_list[2] = {empty};
  size_t _productcount;
  size_t _assemblytime;

public:
  cb_worker(std::shared_ptr<cb_components> &_compobj, size_t assem_time)
      : _component(_compobj), _assemblytime(assem_time) {}
  bool has_item(cb_component_list _item) {
    return _worker_item_list[0] == _item || _worker_item_list[1] == _item;
  }
  bool has_final_product() {
    return _productcount == 0 && has_item(final_product);
  }
  bool needs_item(cb_component_list _item) {
    for (auto &reqc : cb_components::getreq_components()) {
      if (reqc == _item && !has_item(_item))
        return true;
    }
    return false;
  }
  void add_to_worker_list(cb_component_list _item) {
    if (_worker_item_list[0] == empty)
      _worker_item_list[0] = _item;
    else if (_worker_item_list[1] == empty)
      _worker_item_list[0] = _item;
    else
      return;
  }
  void remove_frm_worker_list(cb_component_list _item) {
    if (_worker_item_list[0] == _item)
      _worker_item_list[0] = empty;
    else if (_worker_item_list[1] == _item)
      _worker_item_list[1] = empty;
    return;
  }
  std::pair<cb_component_list, bool>
  startWork(std::shared_ptr<cb_components> comp_obj, bool slot_state) {
    cb_component_list curr_slot_item = comp_obj->get_item();
    bool worker_state = false;
    if (_productcount > 0) {
      _productcount -= 1;
      return {empty, false};
    }
    if (!slot_state)
      return {empty, false};
    if (has_final_product() && comp_obj->can_replace_item(curr_slot_item)) {
      remove_frm_worker_list(final_product);
      return {final_product, true};
    }
    if (comp_obj->can_take_item(curr_slot_item) && needs_item(curr_slot_item)) {
      add_to_worker_list(curr_slot_item);
      worker_state = true;
    }
    if (comp_obj->can_make_product(_worker_item_list)) {
      for (auto &reqc : cb_components::getreq_components()) {
        remove_frm_worker_list(reqc);
      }
      add_to_worker_list(final_product);
      _productcount = _assemblytime;
    }
    return {empty, worker_state};
  }
};
class cb_slots {
private:
  std::shared_ptr<cb_components> component;
  std::shared_ptr<cb_worker> workers[2];

public:
  cb_slots(std::shared_ptr<cb_components> &comp_obj) {
    component = comp_obj;
    for (int i = 0; i < 2; i++)
      workers[i] = std::make_shared<cb_worker>(comp_obj, 3);
  }
  void start_slots() {
    bool slot_state = true;
    for (auto &w : workers) {
      std::pair<cb_component_list, bool> worker_out =
          w->startWork(component, slot_state);
      if (worker_out.second) {
        component->set_item(worker_out.first);
        slot_state = false;
      }
    }
  }
  cb_component_list get_item_frm_slot() {
    cb_component_list slot_item = component->get_item();
    component->set_item(empty);
    return slot_item;
  }
  void put_item_in_slot(cb_component_list _item) { component->set_item(_item); }
};
class cb_conveyor_belt {
private:
  std::shared_ptr<cb_components> _component;
  std::vector<std::shared_ptr<cb_slots>> _slots;
  cb_component_list _input_item;
  std::vector<cb_component_list> _output_item;

public:
  explicit cb_conveyor_belt(std::shared_ptr<cb_components> &comp_obj,
                            size_t numbers_slot)
      : _component(comp_obj) {
    for (int i = 0; i < numbers_slot; i++)
      _slots.push_back(std::make_shared<cb_slots>(_component));
  }
  void start_belt() {
    cb_component_list _next_item;
    cb_component_list _last_item;
    _next_item = _component->genrate_random_component();
    for (auto &s : _slots) {
      s->start_slots();
      _last_item = s->get_item_frm_slot();
      s->put_item_in_slot(_next_item);
      _next_item = _last_item;
    }
    _output_item.push_back(_next_item);
  }

  std::unordered_map<std::string, int> cb_slot_output() {
    std::unordered_map<std::string, int> _result;
    for (auto &it : _output_item) {
      _result[component_tostring[it]] += 1;
    }
    return _result;
  }
};
} // namespace cb_factory

int main() {
  std::shared_ptr<cb_factory::cb_components> component(
      new cb_factory::cb_components(cb_factory::empty));
  std::shared_ptr<cb_factory::cb_conveyor_belt> belt(
      new cb_factory::cb_conveyor_belt(component, 3));
  for (int i = 0; i < 100; i++) {
    belt->start_belt();
  }
  std::unordered_map<std::string, int> res = belt->cb_slot_output();
  for (auto &r : res) {
    std::cout << r.first << "---> " << r.second << "  \n";
  }
}
