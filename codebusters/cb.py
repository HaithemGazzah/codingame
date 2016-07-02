import sys
import math
from collections import namedtuple
from collections import deque
import random


def move_v(c):
    return "MOVE " + str(c[0]) + " " + str(c[1])


def print_move(c):
    print("MOVE " + str(int(c[0])) + " " + str(int(c[1])))

def dist(c1,c2):
    return int(math.sqrt((c1[0]-c2[0])**2 + (c1[1]-c2[1])**2))


Msg_base = namedtuple('Msg',['type','phan'])


Enti = namedtuple('Enti', ['id', 'coord', 'type','state','val'])


#classes which defines action to perform


class GoCenter:
    def __init__(self):
        self.done = False
     
        
    def is_valid(self):
        return True
    
    def perform_action(self):
       
        print_move((8000,4500))
        self.done = True
        


class HelpBust:
    def __init__(self,agent_,msg_):
        self.done = False
        self.agent = agent_
        self.msg = msg_
      
        
    def is_valid(self):
        return self.msg == self.agent.msg
    
    def perform_action(self):
        print("HELP !!",file=sys.stderr)

        if dist(self.agent.bust.coord,self.msg.phan.coord) < 10:
            #ok sur zone
            #action de base on chope phantome
            self.agent.action_planed = TakePhantom(self.agent,self.msg.phan.id)

            
          

            #mais on prefere stun
            if self.agent.last_stun + 20 <= self.agent.round_num and self.agent.view_ents['opo']:
                for opo_id in self.agent.view_ents['opo']:
                     if  self.agent.view_ents['opo'][opo_id].state == 3:
                          self.agent.action_planed = StunOpo(self.agent,opo_id)
                          break
            self.agent.action_planed.perform_action()
            self.done = True      
        else:
            print_move(self.msg.phan.coord)
      

class WhenStunt:
    def __init__(self):
        self.done = False
        
    def is_valid(self):
        return True
    
    def perform_action(self):
        print_move((0,0))
        self.done = True
        
class ReturnHome:
    def __init__(self,agent_,home_):
        self.home = home_
        self.agent = agent_
        self.done = False

    def is_valid(self):
        return self.agent.bust.state == 1 #plus de phan
           
    def perform_action(self):
        print("RETURN HOME",file=sys.stderr)

        if dist(self.agent.bust.coord,self.home) <= 1600:
            print("RELEASE")
            self.done = True
        elif self.agent.last_stun + 20 <= self.agent.round_num and self.agent.view_ents['opo']: #if on voit un mechant
            # Attaque !
            print("STUNT BRICOL",file=sys.stderr)
            found = False
            for opo_id in self.agent.view_ents['opo']:
                #opo_id = next (iter (self.agent.view_ents['opo'].keys()))
                if  self.agent.view_ents['opo'][opo_id].state == 0 and dist(self.agent.view_ents['opo'][opo_id].coord,self.agent.bust.coord) <= 1760:
                    self.agent.action_planed = StunOpo(self.agent,opo_id)
                    self.agent.action_planed.perform_action()
                    found = True
                    break
            if not found: print_move(self.home)
        else:
            print_move(self.home)
        
        


class ExploreMap:
    def __init__(self,agent_,map_explor_):
        self.mx = map_explor_
        self.agent = agent_
        self.done = False

    def is_valid(self):
        return True
           
    def perform_action(self):
        print("ACTION EXPLOR MAP",self.agent.round_num,file=sys.stderr)
        mc = self.mx.default_coord(self.agent.bust.coord)
        print(mc,file=sys.stderr)
        if self.agent.round_num > 100 and mc[0] == -1:
            print("GO OPPO",file=sys.stderr)
            if self.agent.pf.team_id == 0:
                 print_move((13000,7000))
            else:
                 print_move((2300,2000))
        elif self.agent.round_num < 20:
            print_move(mc[2])
        else:
            if mc[0] != -1: print_move(mc[0])
            else: print_move(mc[2])
            
        self.done = True
        
class TakePhantom:
    #2 thing to do : be a the right distance and take the phantom
    def __init__(self,agent_,phanid_):
        self.agent = agent_
        self.phan_id = phanid_
        self.done = False

    def is_valid(self):
        return self.phan_id in self.agent.view_ents['phan']




    
    def perform_action(self):
        print("ACTION TAKE PHAN",file=sys.stderr)
        bust = self.agent.bust
        
        #if self.phan_id in self.agent.view_ents['phan']:
        if self.agent.view_ents['phan']:
            self.phan_id = min(self.agent.view_ents['phan'], key=lambda k: self.agent.view_ents['phan'][k].state) 
            phan = self.agent.view_ents['phan'][self.phan_id]

            if phan.state == 0:
                #need help !
                print("NEED HELP !!",file=sys.stderr)
                self.agent.broadcast_msg(Msg_base("HELP",phan))



            #check si enemie
            if self.agent.last_stun + 20 <= self.agent.round_num and self.agent.view_ents['opo']:
                for opo_id in self.agent.view_ents['opo']:
                    if dist(bust.coord,self.agent.view_ents['opo'][opo_id].coord) <= 1760:
                        self.agent.action_planed = StunOpo(self.agent,opo_id)
                        self.agent.action_planed.perform_action()
                        return
                    
            dist_b_p = dist(bust.coord,phan.coord)
            if dist_b_p < 1760:# and dist_b_p > 900:
                if dist_b_p > 900:
                    print("BUST " + str(phan.id))
                    if self.agent.view_ents['phan'][self.phan_id].state == 1:
                        self.done = True
                else: #on s'eloigne, direction home de preference !
                    print("ELOIGNE ",file=sys.stderr)
                    dist_home = dist(self.agent.home_coord,phan.coord)
                    vx = self.agent.home_coord[0] - phan.coord[0]
                    vy = self.agent.home_coord[1] - phan.coord[1]

                    vx_new = vx/dist_home*1000
                    vy_new = vy/dist_home*1000
                    print_move((phan.coord[0] +vx_new, phan.coord[1] +vy_new))
            else:
                #on sapproche
                print_move(phan.coord)
                            
        else:
            #on a perdu le phantome
            print("NIMPE ",file=sys.stderr)
            print_move((0,0))
            self.done = True


class StunOpo:
    #2 thing to do : be a the right distance and stun
    def __init__(self,agent_,opoid_):
        self.agent = agent_
        self.opo_id = opoid_
        self.done = False

    def is_valid(self):
        return True

    def perform_action(self):
        print("ACTION STUN",file=sys.stderr)
        bust = self.agent.bust
        opo = self.agent.view_ents['opo'][self.opo_id]
        dist_b_p = dist(bust.coord,opo.coord)
        if dist_b_p < 1760:
            print("STUN " + str(opo.id))
            self.done = True
            self.agent.last_stun = self.agent.round_num
        else:
            #on sapproche
            print_move(opo.coord)

class Plateform_system:
    def __init__(self,num_agent_,team_id_):
        self.num_agent = num_agent_
        self.team_id = team_id_
        self.agents = {}
        self.first_found = False


    def connect_agent(self,ag_):
        self.agents[ag_.play_id] = ag_
        if not self.first_found:
           self.agents[ag_.play_id].first_bust = True
           self.first_found = True

    def broadcast_msg(self,sender,msg):
        for ag in self.agents:
            if self.agents[ag] is sender: continue
            self.agents[ag].queue.append(msg)

    def update_entities(self,en_l,round_num_):
        #first update the status of the bust
        for e in en_l:
            if e.type == self.team_id:
                self.agents[e.id].bust = e
                self.agents[e.id].view_ents['phan'] = {}
                self.agents[e.id].view_ents['opo'] = {}
                self.agents[e.id].round_num = round_num_
        print(self.agents,file=sys.stderr)
        #now vievable object
        for e in en_l:
            if e.type == self.team_id: continue
            for id_ag in self.agents:
                ag = self.agents[id_ag]
                if dist(e.coord,ag.bust.coord) <= 2200:#viewable
                    if e.type == -1:
                        ag.view_ents['phan'][e.id] = e
                    else:
                        ag.view_ents['opo'][e.id] = e
            
class Agent:
    def __init__(self,map_ex_,pf_,play_id_,home_coord_):
        self.pf = pf_
        self.map_ex = map_ex_
        self.home_coord = home_coord_
        self.play_id = play_id_
        
        self.queue = deque()
        self.view_ents = {'phan': {}, 'opo': {}}
        self.bust = 0
        self.action_planed = 0
        self.round_num = 0
        self.last_stun = -20
        self.first_bust = False

        self.msg = False #no msg
        
        self.pf.connect_agent(self)
        
    def broadcast_msg(self,msg):
        self.pf.broadcast_msg(self,msg)

    def send_msg(self,msg,bot_id):
        bot_id.append(msg)

        
    def get_avail_msg(self):
        ret = 0
        try:
            ret = self.queue.pop()
        except IndexError:
            return False
        else:
            return ret

    def can_stun(self):
        return self.last_stun + 20 <= self.round_num

    def prepare_action(self):
        #update the action (if done, 0)


        self.msg = self.get_avail_msg()

    

        if self.action_planed != 0 and (self.action_planed.done or not self.action_planed.is_valid()):
            self.action_planed = 0



        if self.bust.state == 2: #assomé
            self.action_planed = WhenStunt() #anyway...
            return True

        if self.action_planed != 0:
            return True #ok on sait quoi faire
        
        #++++++ different phase
        if  self.round_num <= 10:
            ###### PHASE 1
            print("Phase 1",file=sys.stderr)
            self.action_planed = ExploreMap(self,self.map_ex) #default action
            
            if self.bust.state == 1: #transport phantom
                self.action_planed = ReturnHome(self,self.home_coord)
                
            elif self.view_ents['phan']:
               
                phan_id = min(self.view_ents['phan'], key=lambda k: self.view_ents['phan'][k].state) 
                if  self.view_ents['phan'][phan_id].state <= 3:
                    self.action_planed = TakePhantom(self,phan_id)
            
           
            return True
        
        elif self.round_num < 300:
            print("Phase 2",file=sys.stderr)
            print("action0",self.view_ents,file=sys.stderr)
            print(self.last_stun,self.round_num,self.view_ents['opo'],file=sys.stderr)
            
            if self.bust.state == 1: #transport phantom
                self.action_planed = ReturnHome(self,self.home_coord)
            elif self.view_ents['phan']: #ok not empty, take phantom
                print("on prend phan MIN",file=sys.stderr)
                phan_id = min(self.view_ents['phan'], key=lambda k: self.view_ents['phan'][k].state) 
                #for phan_id in self.view_ents['phan']:
                self.action_planed = TakePhantom(self,phan_id)
                #    break
            elif self.msg and self.msg.type == "HELP":
                print("**on repond au HELP",file=sys.stderr)
                self.action_planed = HelpBust(self,self.msg)
            elif self.can_stun() and self.view_ents['opo']: #ok not empty, bust enemi
                print("on va stuné ! ",file=sys.stderr)
                for opo_id in self.view_ents['opo']:
                    if self.view_ents['opo'][opo_id].state == 1: #have phantom
                        self.action_planed = StunOpo(self,opo_id)
                    elif self.view_ents['opo'][opo_id].state == 3: #vise, priorité donc
                        self.action_planed = StunOpo(self,opo_id)
                        break
                    else:
                        self.action_planed = ExploreMap(self,self.map_ex)
            else: #on se balade
                self.action_planed = ExploreMap(self,self.map_ex)
                 
            return True
        
        else:
            print("Finale Phase",file=sys.stderr)
            return True
        

    def print_action(self):
        self.action_planed.perform_action()

        
class Map_explore:
    def __init__(self,team_id):

        self.tid = team_id
        self.map =   [{'explor':0,'num_phan':0,'num_bust':0,'list_phan':[]}  for x in range(8*5)]
        if team_id == 0:
            self.map[0] = {'explor':1,'num_phan':0,'num_bust':0,'list_phan':[]} 
                 
        else:
            self.map[7*5] = {'explor':1,'num_phan':0,'num_bust':0,'list_phan':[]} 
                 
    def print_map(self):
        for y in range(5):
            for x in range(8):
               elt = self.map[y*8+x]
               print('E',elt['explor'],'N',elt['num_phan'], end=";", file=sys.stderr)
            print("", file=sys.stderr)
            
    def coord2zone(self,c):
        x = int(c[0]/2001)  #round to lower int
        y = int(c[1]/2001)

        return self.map[y*8+x]


    def coord2zone_m1(self,c):
        x = int(c[0]/2001)  #round to lower int
        y = int(c[1]/2001)

        return self.map[(4-y)*8+(7-x)]


    def default_coord(self,c):
        #1. find the closest zone with phantom
        #2. find the closest unexplored zone
        
        min_empt = 999999
        zon_empt = -1
        min_phan = 999999
        zon_phan = -1

        zon_phan_minimum = -1
        min_phan_minimum = 999999
        
        for x in range(8):
            for y in range(5):
                zone_coord = (x*2001+1000,y*2001+1000)
                distance = dist(c,zone_coord)
                if self.coord2zone(zone_coord)['num_phan'] > 0 and distance < min_phan:
                   min_phan = distance
                   zon_phan = zone_coord
                if self.coord2zone(zone_coord)['explor'] == 0 and distance < min_empt:
                    min_empt = distance
                    zon_empt = zone_coord

                if self.coord2zone(zone_coord)['list_phan'] and min(self.coord2zone(zone_coord)['list_phan']) + distance/800 < min_phan_minimum:
                    min_phan_minimum = min(self.coord2zone(zone_coord)['list_phan'])
                    zon_phan_minimum = zone_coord        

        return (zon_phan_minimum, zon_phan , zon_empt)
    
        
# Send your busters out into the fog to trap ghosts and bring them home!

busters_per_player = int(input())  # the amount of busters you control
ghost_count = int(input())  # the amount of ghosts on the map
my_team_id = int(input())  # if this is 0, your base is on the top left of the map, if it is one, on the bottom right

#class instance
map_ex = Map_explore(my_team_id)


pls = Plateform_system(busters_per_player,my_team_id)

base = (0,0)

if my_team_id == 1:
    base = (16000,9000)

    
#if team_id = 0 -> player id begins at 0, else begin at play 
agents = [Agent(map_ex,pls,busters_per_player*my_team_id + x,base) for x in range(busters_per_player)]


# game loop


round_num = 0  



    
while True:
    round_num += 1
    entities = int(input())  # the number of busters and ghosts visible to you
    enti = []
    
    for i in range(entities):
        # entity_id: buster id or ghost id
        # y: position of this buster / ghost
        # entity_type: the team id if it is a buster, -1 if it is a ghost.
        # state: For busters: 0=idle, 1=carrying a ghost.
        # value: For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
        entity_id, x, y, entity_type, state, value = [int(j) for j in input().split()]
        enti_add = Enti(entity_id, (x, y), entity_type, state, value)
        enti.append(enti_add )

        #update zone
        map_ex.coord2zone((x,y))['num_phan'] = 0
        map_ex.coord2zone((x,y))['list_phan'] = []
        
        if round_num < 20: #symetrie
            map_ex.coord2zone_m1((x,y))['num_phan'] = 0
            map_ex.coord2zone_m1((x,y))['list_phan'] = []
        
        if entity_type == -1:
            map_ex.coord2zone((x,y))['num_phan'] += 1
            map_ex.coord2zone((x,y))['list_phan'].append(state)
            if round_num < 20: #symetrie
                map_ex.coord2zone_m1((x,y))['num_phan'] += 1
                map_ex.coord2zone_m1((x,y))['list_phan'].append(state) 
        if entity_type == my_team_id: #mon bus
            map_ex.coord2zone((x,y))['explor'] = 1 #on a explore
         #   if round_num < 100: #symetrie
         #       map_ex.coord2zone_m1((x,y))['explor'] = 1 #on a explore

    
    pls.update_entities(enti,round_num)
    map_ex.print_map()


    while not all([ag.prepare_action() for ag in agents]): pass


    for ag in agents:
        print(ag.bust, ag.view_ents,file=sys.stderr)
        ag.print_action()
    #break

   
