import sys, re
import math
#from sympy import Point, Line

#Class to create a point from the given coordinates and convert it to float
class point:
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)
    def __repr__(self):
        return f"({self.x:.2f}, {self.y:.2f})"
        
#Class to create a line from the points
class line:
    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2
    def __repr__(self):
        return f"{self.p1} ---> {self.p2}" 

#Class for doing arithmetic operations
class arithmetic_formulaes(object):
    
    #Get distance between 2 points
    def distance(self, p1, p2):
        return math.sqrt((p1.x - p2.x)**2 + (p1.y - p2.y)**2)

    #Check if the point is lying between  2 points
    def is_between(self, a,c,b):
        return round(self.distance(a,c) + self.distance(c,b),2) == round(self.distance(a,b),2)
    
    #Find the slope of a line
    def gradiant(self, l):
        if (l.p1.x) != (l.p2.x):
            return (1./(l.p1.x-l.p2.x) * (l.p1.y - l.p2.y)) 
            # print("Slope :", slope)
            # return slope
         #return(1./(l[1][0]-l[0][0]) * (l[1][1] - l[0][1]))
    
    #Check if 2 lines are parallel or not
    def parallel(self, l1, l2):
        return True if self.gradiant(l1) == self.gradiant(l2) else False        
    
    #calculate the intercept of the line
    def intercept(self, l):
        return (l.p1.y - self.gradiant(l) * l.p1.x)
        # print("Intersept: ", +intercept)
    
    #We have used Slope intercept form through out this code. but the intersection point can also be calculated using cramers rule. 
    def intersection(self, l1, l2):
        # print("dfghjk", self.parallel(l1, l2))
        if self.parallel(l1, l2) == False:
            # print("parallel successfull")
            # print(self.gradiant(l1))
            # print(self.gradiant(l2))
            if self.gradiant(l1) is not None and self.gradiant(l2) is not None:
                x = (1./(self.gradiant(l1) - self.gradiant(l2)) * (self.intercept(l2) - self.intercept(l1)))
                y = (self.gradiant(l1) * x) + self.intercept(l1)
                
            if self.gradiant(l1) is None:
                # print(l1.p1.x)
                x = l1.p1.x
                y = (self.gradiant(l2) * x) + self.intercept(l2)
                #print(x)
                
            if self.gradiant(l2) is None:
                x = l2.p2.x
                y = (self.gradiant(l1) * x) + self.intercept(l1)
                #print(x, y)
            
        return point(x, y)
    

class get_output_graph(object):

    street_line_dict = {}
    
    #Constructor to take street dictionary as input
    def __init__(self, streets_dict):
        self.street_line_dict = streets_dict
        self.get_vertices = []
        self.get_intersection_list = []
        self.get_vertices_map = {}
        self.edge_list = []
        self.line_map = {}
    
        # print("class dict", self.street_line_dict)
    
    #Method to create line from Street_Dictionary
    def create_line(self, street_line_dict):
        for street, coordinates in self.street_line_dict.items():
            for i in range(len(coordinates)-1):
                if i< len(self.street_line_dict[street]):
                    p1 = point(coordinates[i][0], coordinates[i][1])
                    p2 = point(coordinates[i+1][0], coordinates[i+1][1])
                
                    line_store = line(p1,p2)
                    self.line_map.setdefault(street,[]).append(line_store)
                else:
                    break
        # print ("Line Map", self.line_map)
        return self.line_map
    
    #find the vertices
    def getVertices(self):
        # print("dfghjkl", self.street_line_dict)
        self.create_line(self.street_line_dict)
        # print("ttttttttt", self.create_line(self.street_line_dict))
        
        index = 0
        
        for street_name1 in self.line_map: #weber street
            #print(street_name1)
            #print(len(self.line_map[street_name1])) #4
            for i in range(len(self.line_map[street_name1])): #i=0 to 3
                line1 = self.line_map[street_name1][i]  #(2.00, -1.00) ---> (2.00, 2.00)
                # print("Line1: ", line1)
                for street_name2 in self.line_map:  
                    if street_name2 == street_name1:
                        continue
                    for j in range(len(self.line_map[street_name2])):
                        line2 = self.line_map[street_name2][j]  #(4.00, 2.00) ---> (7.00, 8.00)
                        # print("Line2", line2)
                        arithmetic = arithmetic_formulaes()
                        # print("Line1", line1)
                        # print("Line2", line2)
                        # print("Arithmetic_parallel", arithmetic.parallel(line1, line2))
                        if not arithmetic.parallel(line1, line2):   #False
                            #print("Not Parallel")
                            intersection  = arithmetic.intersection(line1, line2)
                            #print(intersection)
                            v1, v2, vi, v4, v5 = line1.p1, line1.p2, intersection, line2.p1, line2.p2
                            
                            #print("Intersection Vertices", (v1, v2, vi, v4, v5))

                            # print(arithmetic.is_between(v1, vi, v2))
                            # print(arithmetic.is_between(v5, vi, v4))
                            if arithmetic.is_between(v1, vi, v2) and arithmetic.is_between(v5, vi, v4):
                                # v1 = f"({v1.x:.2f}, {v1.y:.2f})"
                                v1, v2, vi, v4, v5 = [f'{v1.x:.2f}', f'{v1.y:.2f}'], [f'{v2.x:.2f}', f'{v2.y:.2f}'], [f'{vi.x:.2f}', f'{vi.y:.2f}'], [f'{v4.x:.2f}', f'{v4.y:.2f}'], [f'{v5.x:.2f}', f'{v5.y:.2f}'] 
                                
                                # print(v1, v2, vi, v4, v5)
                                
                                if vi not in self.get_intersection_list:
                                    self.get_intersection_list.append(vi)
                                    
                                temp = [vi, v1, v2, v4, v5]
                                    
                                for values in range(len(temp)):
                                    if temp[values] not in self.get_vertices_map.values():
                                        self.get_vertices_map[index+1] = temp[values]
                                        index+=1
        # print("Intersection_List", self.get_intersection_list)   #Intersection_List [['4.00', '4.00'], ['4.00', '7.00']]
        # print (self.get_vertices_map)                                   
        return self.get_vertices_map                                    
        
    #Calculate the edges
    def getEdges(self):
        for streetname in self.line_map:
            # print(streetname)   #Weber
            for i in range(len(self.line_map[streetname])):
                intersec_points = []
                line1 = self.line_map[streetname][i]
                lp1, lp2 = line1.p1, line1.p2
                # print("Line Point1", lp1)   #Line Point1 (4.00, 2.00)
                # print("Line Point2", lp2)   #Line Point2 (4.00, 8.00)   
                
                p1 = [f'{lp1.x:.2f}', f'{lp1.y:.2f}']
                p2 = [f'{lp2.x:.2f}', f'{lp2.y:.2f}']
                
                arithmetic = arithmetic_formulaes()
                
                for vertices in self.get_vertices_map:
                    if p1 == self.get_vertices_map[vertices]:
                        p1_index = vertices
                        # print("p1_index", p1_index)    p1_index 4
                for vertices in self.get_vertices_map:
                    if p2 == self.get_vertices_map[vertices]:
                        p2_index = vertices
                        # print("p2_index", p2_index)    p2_index 5
                        
                for intersection in self.get_intersection_list:
                    c = point(intersection[0], intersection[1])
                    if arithmetic.is_between(lp1, c, lp2): 
                        intersec_points.append(c)   #get intersection points  [[4.00, 4.00], [4.00, 7.00]]
                        # print("intersec_points", intersec_points)  intersec_points [(4.00, 4.00), (4.00, 7.00)]
                    else:
                        continue
                    
                sorted_intersec_points = sorted(intersec_points, key=lambda e: arithmetic.distance(e, lp1))
                # print("Sorted Points: ", sorted_intersec_points)   Sorted Points:  [(4.00, 4.00), (4.00, 7.00)]
                sorted_intersec_index = [vert for j in sorted_intersec_points for vert, vert_coords in self.get_vertices_map.items() if [f'{j.x:.2f}', f'{j.y:.2f}'] ==  vert_coords]
                # print("Sorted Index1: ", sorted_index)

                if sorted_intersec_index:
                    sorted_intersec_index = [p1_index] + sorted_intersec_index + [p2_index]
                    # print(sorted_index)  [5, 1, 6, 4]
                    for f in range(len(sorted_intersec_index)-1):
                        temp=[sorted_intersec_index[f],sorted_intersec_index[f+1]]
                        # print("Temp: ", temp)  Temp:  [5, 1] [1, 6] [6 4]
                        if (sorted_intersec_index[f] != sorted_intersec_index[f+1]) and temp != self.edge_list:
                            self.edge_list.append([sorted_intersec_index[f],sorted_intersec_index[f+1]])
                            # print(self.edge_list)  [[3, 1], [1, 2], [8, 6], [6, 7], [5, 1], [1, 6], [6, 4]]
        return (self.edge_list)

def main():
    streets_dict = {}
     #Initiating street name Dictionary  
     
    '''
     Sample Input
     a "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)
     a "King Street S" (4,2) (4,8)
     a "Davenport Road" (1,4) (5,8)
     c "Weber Street" (2,1) (2,2)
     c "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)
     r "King Street S"
    '''
     
    while(True):
        try:
            street_input = sys.stdin.readline()
            # try: 
            #     # street_input = input()#Question Why is sys.stdin.readline is not working?
            #     street_input = sys.stdin.readline()
            #     # street_input = street_input.replace('\n', '')
                
            #     # if street_input == '':
            #     #     break
        except EOFError:
            sys.exit(0)
            break
   
        #Defining regex in 3 blocks as per the input 
        #?: any group/non-capturing group
        #+? This means it tries to match as few times as possible, instead of trying to match as many times as possible
        #*? means "match the previous element as few times as possible (zero or more times)".
        input_regex = r'( *?[acrg])( +?\"[a-zA-Z ]+?\"*?)? *?((?: +?\( *?[-]?\d+? *?, *?[-]?\d+? *?\) *?)*) ?$'
        # command_input_regex = r'( *?[acrg])'
        # street_name_Regex = r'( +?\"[a-zA-Z ]+?\"*?)?'
        # coordinates_regex = r' *?((?:\( *?[-]?[0-9]+? *?, *?[-]?[0-9]+? *?\) *?)*) ?$'
        formatted_input = re.match(input_regex, street_input)
        print(formatted_input)
        # try:
        #     formatted_input = re.match(input_regex, street_input)
        # except Exception:
        #     print("Input Value is not in proper format\n")
        #     sys.exit(0)
        #     break
        
        #print(commands)
        if formatted_input:
            # if commands == 'a' or 'c' or 'r':
            commands = formatted_input.group(1)
            commands = commands.strip()
            street_name = formatted_input.group(2)
            try:
                raw_coordinates = formatted_input.group(3)
            except:
                pass 
            #print(commands)
            #print(street_name)
            #print(raw_coordinates)  
            # coordinates = re.findall(r'[-]?\d+',raw_coordinates)
            # coordinates = [[coordinates[i], coordinates[i + 1]] for i in range(len(coordinates) - 1)]
            # del coordinates[1::2]
        else:   
            pass
            # sys.stderr.write("Error: Invalid Input format\n")
            # continue
        
        coordinates = re.findall(r'[-]?\d+',raw_coordinates)
        coordinates = [[coordinates[i], coordinates[i + 1]] for i in range(len(coordinates) - 1)]
        del coordinates[1::2]
        
        
        if commands != 'g':
            street_name = street_name.replace("\"", '')
            street_name = street_name.lower()
            street_name = street_name.lstrip()
        #To add the streets*
        if commands == 'a':
            if street_name not in streets_dict:
                streets_dict[street_name] = coordinates
                # print(streets_dict)
            
        #To change/update the streets
        if commands == 'c':
            try:
                if street_name in streets_dict:
                    streets_dict[street_name] = coordinates
                    # print(streets_dict)
            except Exception:
                pass
                # sys.stdout.write("Error: ’c’ or ’r’ specified for a street that does not exist.\n")
            
        #To remove the streets
        elif commands == 'r':
            try:
                del streets_dict[street_name]
            except Exception:
                pass
                # sys.stdout.write("Error: ’c’ or ’r’ specified for a street that does not exist.\n")
            # print(streets_dict)
            
        #generate the output

        elif commands == 'g':
            #output()
            output_graph = get_output_graph(streets_dict)
            final_vertices = output_graph.getVertices()
            # output_graph.create_line()
            # print("output graph: ", output_graph)
            # print ("V = {")
            # for num in final_vertices:
            #     print("{0}: ({1}, {2})".format(str(num), str(final_vertices[num][0]), str(final_vertices[num][1])))
            # print ("}")
            
            vertex_length = len(final_vertices)
            # if vertex_length != 0:
            #     sys.stdout.write("V "+str(vertex_length)+"\n")
            # # print(output_graph.getEdges())
            #     final_edges = output_graph.getEdges()
            #     sys.stdout.write("E {")
            #     for num, value in enumerate(final_edges):
            #         # print(num)
            #         # print(value)
            #         if num != (len(final_edges)-1):
            #         # print ("<"+str(num[0])+","+str(num[1])+">"+",")
            #             sys.stdout.write("<{0},{1}>,".format(str(value[0]) , str(value[1])))
            #         else:
            #             sys.stdout.write("<{0},{1}>".format(str(value[0]) , str(value[1])))
            #     sys.stdout.write ("}")

            if vertex_length != 0:
                sys.stdout.write("V " + str(vertex_length+1) + "\n")
                final_edges = output_graph.getEdges()
                # print("final_edges:", final_edges)
                edge_str = ",".join("<{},{}>".format(str(e[0]), str(e[1])) for e in final_edges)
                # print("edge_str:", edge_str)
                sys.stdout.write("E {" + edge_str + "}" + "\n")
                sys.stdout.flush()
                                         
        # except Exception as exp:
        #     sys.stdout.write("Error: " +exp + "\n")
        #     sys.stdout.flush()
            
            
if __name__ == '__main__':
    main()
