from fractions import Fraction
import math
from sys import argv

COUNT = False

def subtract_points(p1, p2):
    """Subtracts point p2 from p1."""
    return (p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2])

def dot_product(v1, v2):
    """Calculates the dot product of two vectors."""
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]

def cross_product(v1, v2):
    """Calculates the cross product of two vectors."""
    x = v1[1] * v2[2] - v1[2] * v2[1]
    y = v1[2] * v2[0] - v1[0] * v2[2]
    z = v1[0] * v2[1] - v1[1] * v2[0]
    return (x, y, z)

def are_vectors_parallel(v1, v2):
    """Checks if two vectors are parallel."""
    # Two vectors are parallel if their cross product is the zero vector.
    cp = cross_product(v1, v2)
    return cp[0] == 0 and cp[1] == 0 and cp[2] == 0

def line_segments_intersect(A, B, C, D):
    """
    Checks if the open line segments AB and CD have a non-empty intersection.
    Points are given as tuples of (x, y, z) integer coordinates.
    """
    v_AB = subtract_points(B, A)
    v_CD = subtract_points(D, C)
    v_AC = subtract_points(C, A)

    # Calculate the normal vector to the plane containing v_AB and v_CD
    # This is useful for determining if the lines are skew.
    n = cross_product(v_AB, v_CD)
    
    # If n is the zero vector, the lines are parallel or collinear.
    if n == (0, 0, 0):
        # The lines are parallel.
        # Check if they are collinear by seeing if AC is also parallel to v_AB.
        if not are_vectors_parallel(v_AC, v_AB):
            return False # Parallel and not collinear, so no intersection.
        
        # Lines are collinear. Now check for overlap of segments.
        # Project points onto the line and check for overlap in 1D.
        # We can use dot products.
        len_sq_AB = dot_product(v_AB, v_AB)
        len_sq_CD = dot_product(v_CD, v_CD)

        # Handle cases where segments are just points (though problem guarantees distinct points)
        if len_sq_AB == 0 or len_sq_CD == 0:
            return False 

        # Parameters for C and D on line AB, where origin is A and direction is v_AB
        t_C = Fraction(dot_product(v_AC, v_AB), len_sq_AB)
        t_D = Fraction(dot_product(subtract_points(D, A), v_AB), len_sq_AB)

        # Normalize t_C and t_D to be between 0 and 1 for segment CD relative to AB
        # This simplifies checking for overlap.
        # We need to find the interval [min(t_C, t_D), max(t_C, t_D)] for segment CD,
        # and see if it overlaps with (0, 1) for segment AB.
        
        # Interval for segment AB is (0, 1)
        # Interval for segment CD on the line of AB is (min(t_C, t_D), max(t_C, t_D))
        
        min_t_CD = min(t_C, t_D)
        max_t_CD = max(t_C, t_D)
        
        # Check for overlap: max(start1, start2) < min(end1, end2)
        # For open segments (0, 1) and (min_t_CD, max_t_CD)
        # Intersection requires:
        #   max(0, min_t_CD) < min(1, max_t_CD)
        #   and the intervals are non-empty.
        
        overlap_start = max(Fraction(0), min_t_CD)
        overlap_end = min(Fraction(1), max_t_CD)
        
        return overlap_start < overlap_end

    # If the scalar triple product (v_AC . n) is non-zero, the lines are skew.
    # This means C is not in the plane defined by A, v_AB and v_CD.
    if dot_product(v_AC, n) != 0:
        return False # Lines are skew, no intersection.

    # Lines are not parallel and not skew, so they must intersect at a single point.
    # We can solve for t and u using Cramer's rule or by picking two equations.
    # Let's use the cross product method for t and u, which is robust.
    # t = (v_AC . cross(v_CD, v_AB)) / (v_AB . cross(v_CD, v_AB))
    # u = (v_AC . cross(v_CD, v_AB)) / (v_AB . cross(v_CD, v_AB))
    
    # A cleaner approach using vector algebra:
    # t * v_AB - u * v_CD = v_AC
    # Take cross product with v_CD:
    # t * (v_AB x v_CD) = v_AC x v_CD
    # t * n = v_AC x v_CD
    # Take dot product with n:
    # t * (n . n) = (v_AC x v_CD) . n
    # t = ((v_AC x v_CD) . n) / (n . n)
    
    n_squared = dot_product(n, n)
    
    # Numerator for t: dot(cross(v_AC, v_CD), n)
    num_t_vec = cross_product(v_AC, v_CD)
    num_t = dot_product(num_t_vec, n)
    
    # Numerator for u: dot(cross(v_AC, v_AB), n)
    num_u_vec = cross_product(v_AC, v_AB)
    num_u = dot_product(num_u_vec, n)
    
    t = Fraction(num_t, n_squared)
    u = Fraction(num_u, n_squared)

    # Check if t and u are within the open interval (0, 1)
    # Using Fraction(0) and Fraction(1) ensures exact comparison
    return Fraction(0) < t < Fraction(1) and Fraction(0) < u < Fraction(1)

if __name__ == '__main__':
    
    assert len(argv)==2, f'correct usage: python3 {argv[0]} contestants_output'

    N = 100
    MAXABC = 1000

    try:    
        try:
            outfile = open(argv[1],'r')
            outlines = outfile.readlines()
        except:
            assert False, 'Failed to parse the output (probably not a valid text file).'

        assert len(outlines) == 2*N + 1, 'Output must have 1 + 2*N lines.'

        try:
            A, B, C = [ int(_) for _ in outlines[0].split() ]
        except:
            assert False, 'The first line must contain the integers a, b, c.'

        assert 0 < A and 0 < B and 0 < C, 'The integers a, b, c must be positive.'
        assert A <= MAXABC and B <= MAXABC and C <= MAXABC, f'The integers a, b, c must not exceed {MAXABC}.'

        try:
            points = [ [ int(_) for _ in row.split() ] for row in outlines[1:] ]
        except:
            assert False, 'Each line of output must contain three integers.'

        for pt in points:
            assert len(pt) == 3, 'Each line of output must contain three integers.'
            assert 0 <= pt[0] < A and 0 <= pt[1] < B and 0 <= pt[2] < C, 'Coordinates of each point must be in [0,a-1]x[0,b-1]x[0,c-1].'

        assert len(set([ tuple(pt) for pt in points ])) == 2*N, 'No duplicate points allowed.'

        red, blue = points[:N], points[N:]
        intersections = 0
        for R1 in red:
            for R2 in red:
                if R1 >= R2: continue
                for B1 in blue:
                    for B2 in blue:
                        if B1 == B2: continue
                        if COUNT:
                            if line_segments_intersect(R1, B1, R2, B2): intersections += 1
                        else:
                            assert not line_segments_intersect(R1, B1, R2, B2), 'Some line segments intersect.'

        if COUNT:
            assert intersections == 0, f'{intersections} pairs of segments intersect.'
            
    except Exception as error_message:
        print(error_message)
        exit(1)
        
    volume = A*B*C
    score = 10 + 150000000. / math.pow(volume, 1.5)
    score = min( 100, max( 0, round(score,2) ) )

    print(f'{score:.2f}')
    print('OK')
    exit(0)

