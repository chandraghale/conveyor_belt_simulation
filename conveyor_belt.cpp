#include <bits/stdc++.h>

namespace cb_factory {
/** list of item that can be in slot **/
enum cb_component_list { empty = 0, component_a, component_b, final_product };

/** maping component to string name **/
std::map<cb_component_list, std::string> component_tostring = {
    {empty, "No Item "},
    {component_a, "Component A"},
    {component_b, "Component B"},
    {final_product, "Product P"}
};
/** 
    class cb_components : Describe the current component in the slot
**/
class cb_components {
private:
  cb_component_list _component;

public:
  cb_components() {}
  explicit cb_components(cb_component_list _item) : _component(_item) {}
  static std::vector<cb_component_list> getreq_components();

  bool can_take_item(cb_component_list _item) {
     /* can take either component A or B to make final product */
    return _item == component_a || component_b;
  }
  bool can_replace_item(cb_component_list _item) { 
      /* can only replace the item with empty slot */
      return _item == empty;
    }
  bool can_make_product(cb_component_list _worker_item_list[]) {
    /* check if the list of items with workers contains all required componnents
        to make final product ( both A and B should be in list)
    */

    bool flag = false;
    for (auto &reqc : cb_components::getreq_components()) {
      if (_worker_item_list[0] == reqc || _worker_item_list[1] == reqc) {
        flag = true;
        continue;
      }
      flag = false;
      if (!flag)
        return false;
    }
    return true;
  }
  /*
    @ get_item : returns the current component in slot
    @ set_item : set the current component in slot
  */
  cb_component_list get_item() const { return _component; }
  const void set_item(cb_component_list curr_item) { _component = curr_item; }

  /*
    @cb_component_list genrate_random_component : generate random component
     either ( A or B or empty )with equal probablity.
  */
  cb_component_list genrate_random_component() const {
    return static_cast<cb_component_list>(rand() % 3);
  }
};
/*
    getreq_components : return list of components required to make final product
*/
 std::vector<cb_component_list> cb_components::getreq_components()  {
  return {component_a, component_b};
}

/** 
    Class cb_worker : Describe workers task on the given slot
**/
class cb_worker {
  std::shared_ptr<cb_components> _component;
  cb_component_list _worker_item_list[2] = {empty}; // list of items with current worker
  size_t _timer;
  size_t _time_reqd;

public:
  cb_worker(std::shared_ptr<cb_components> &_compobj, size_t _ticker)
      : _component(_compobj), _time_reqd(_ticker) {}
  /*
    @ has_item : return true if the current item is slot is present in
                 worker's list.
  */
  bool has_item(cb_component_list _item) {
    return _worker_item_list[0] == _item || _worker_item_list[1] == _item;
  }
  /*
    @ has_final_product : return true if worker's list contains final product 
  */
  bool has_final_product() {
    return _timer == 0 && has_item(final_product);
  }
  /*
    @ needs_item : return true if the current component is needed  
  */
  bool needs_item(cb_component_list _item) {
    for (auto &reqc : cb_components::getreq_components()) {
      if (reqc == _item && !has_item(_item))
        return true;
    }
    return false;
  }
  /*
    @ needs_item : add the current component in worker's list
  */
  void add_to_worker_list(cb_component_list _item) {
    if (_worker_item_list[0] == empty)
      _worker_item_list[0] = _item;
    else if (_worker_item_list[1] == empty)
      _worker_item_list[1] = _item;
    else{
        #ifdef DEBUG_ON
            std::cerr<< "remove_frm_worker_list() : something wrong" <<"\n";
        #endif
    }
    return;
  }
  /*
    @ needs_item : remove the current component in worker's list and set it to empty
  */
  void remove_frm_worker_list(cb_component_list _item) {
    if (_worker_item_list[0] == _item)
      _worker_item_list[0] = empty;
    else if (_worker_item_list[1] == _item)
      _worker_item_list[1] = empty;
    else{
        #ifdef DEBUG_ON
            std::cerr<< "remove_frm_worker_list() : something wrong"<<"\n";
        #endif
    }
    return;
  }
  /*
    @ start_work : Implements the logic for worker handling the component in the slot
  */
  bool 
  start_work(std::shared_ptr<cb_components> &comp_obj, bool slot_state) {
    cb_component_list curr_slot_item = comp_obj->get_item();
    #ifdef DEBUG_ON
            std::cerr<< "start_work(): worker = " << this << " component" << curr_slot_item <<"\n";
    #endif
    // case 1  
    // time ticker check if worker is busy on production
    if (_timer > 0) {
      _timer--;
       return false;
    }
    if (!slot_state){
        return false;
    }
    // case 2 : component is available and may be final product
    if (has_final_product() && comp_obj->can_replace_item(curr_slot_item)) {
      remove_frm_worker_list(final_product);
      comp_obj->set_item(final_product);
      return true;
    }
    // case 3 : component A or B is avalaible to be picked
    if (comp_obj->can_take_item(curr_slot_item) && needs_item(curr_slot_item)) {
      add_to_worker_list(curr_slot_item);
       comp_obj->set_item(empty);
    }
    // case 4 : final check the list of item with worker to make product
    if (comp_obj->can_make_product(_worker_item_list)) {
      for (auto &reqc : cb_components::getreq_components()) {
        remove_frm_worker_list(reqc);
      }
      add_to_worker_list(final_product);
      _timer = _time_reqd;
      
    }
    return true;
  }
};
/** 
    class cb_slot : Describe the slot state in conveyor belt
**/
class cb_slots {
private:
  std::shared_ptr<cb_components> component;
  std::shared_ptr<cb_worker> workers[2]; // two worker in each slot

public:
  cb_slots(std::shared_ptr<cb_components> &comp_obj) {
    component = comp_obj;
    for (int i = 0; i < 2; i++)
      workers[i] = std::make_shared<cb_worker>(comp_obj, 3);
  }
  /*
    @start_slots :  start work for each worker in current slot
                    checks state of slots in each iteration and checks state of slot
                    and worker if work is already in progress 
  */
  void start_slots() {
    bool slot_state = true;
    for (auto &w : workers) {
      bool curr_state = 
          w->start_work(component, slot_state);
       if (curr_state) {
        component->set_item(component->get_item());
        slot_state = false;
      }
    }
  }
  /*
    @get_item_frm_slot :  get the item from the slot
    @put_item_in_slot :  put the item in the slot
  */
  cb_component_list get_item_frm_slot()  {
    cb_component_list slot_item = component->get_item();
    component->set_item(empty);
    return slot_item;
  }
  void put_item_in_slot(cb_component_list _item)  { 
      component->set_item(_item);
    }
};
/** 
    class cb_conveyor_belt : Describe the Conveyor Belt state. 
**/
class cb_conveyor_belt {
private:
  std::shared_ptr<cb_components> _component; // current component object 
  std::vector<std::shared_ptr<cb_slots>> _slots; // slots present in belt
  cb_component_list _input_item; // current component generated
  std::vector<cb_component_list> _output_item; // output after iteration

public:
  explicit cb_conveyor_belt(std::shared_ptr<cb_components> &comp_obj,
                            size_t numbers_slot)
      : _component(comp_obj) {
    for (int i = 0; i < numbers_slot; i++)
      _slots.push_back(std::make_shared<cb_slots>(_component));
  }
  /*
    @start_belt : starts all the slots, and update  move component to 
                  the item in each iteration, at the end of iteration 
                  capture the output of belt.
  */
  void start_belt() {
    cb_component_list _next_item;
    cb_component_list _last_item;
    _next_item = _component->genrate_random_component();
    #ifdef DEBUG_ON
        std::cerr<<"genrate_random_component() returned :  "<<_next_item<<"\n";
    #endif
    for( auto &s : _slots){
        #ifdef DEBUG_ON
            std::cerr<<"start_belt() component in slot "<<this<<" = " << s->get_item_frm_slot() << "\n";
        #endif
        s->put_item_in_slot(_next_item);
        s->start_slots();
        // update the slot item
        _last_item = s->get_item_frm_slot();
        _next_item = _last_item;
    }
    _output_item.push_back(_next_item);
  }
    /*
        @cb_slot_output : capture the ouput of belt in a map.
    */
  std::unordered_map<std::string, int> cb_slot_output() {
    std::unordered_map<std::string, int> _result;
    for (auto &it : _output_item) {
      _result[component_tostring[it]] += 1;
    }
    return _result;
  }
};
} // namespace cb_factory

/* Drivers code */

int main() {
  int _number_slot = 3;
  int _number_iteration = 100;
  std::shared_ptr<cb_factory::cb_components> component =
                    std::make_shared<cb_factory::cb_components>(cb_factory::empty);
  std::shared_ptr<cb_factory::cb_conveyor_belt> belt = 
      std::make_shared<cb_factory::cb_conveyor_belt>(component, _number_slot);
  for (int i = 0; i < _number_iteration; i++) {
    belt->start_belt();
  }
  std::unordered_map<std::string, int> res = belt->cb_slot_output();
  #ifdef DEBUG_ON
    std::cout<<"{"<<std::endl;
    for (auto &r : res) {
        std::cout << r.first << ": " << r.second << "\n";
    }
    std::cout<<"}"<<std::endl;
  #endif
  std::cout<<"Number of final product generated : "<<res["Product P"] <<std::endl;
  std::cout<<"Number of components unused : "<<res["Component A"] + res["Component B"]<<std::endl;
}