
void saveTriangulationToOFF(char* filename, TDS& tds)
{
//	ostream& os = cout;
	ofstream os;
	os.open(filename);

	os << "OFF\n";
	os << tds.number_of_vertices()-1<<" "<< tds.number_of_faces()<<" 0\n"; // number of points, number of faces, number of lines;
//	os << "# list of points: X, Y and Z coordinates"<<endl;

	CGAL::Unique_hash_map<TDS::Vertex_handle,int> V;
	// first vertex 
	Point p;
	int inum = 0;

	// other vertices
	if (tds.number_of_vertices() >1 )
	{
		TDS::Vertex_iterator vit= tds.vertices_begin();
		V[vit] = inum++;
		p = vit->point();
		//os << p.x() <<" "<< p.y() <<" 0" << std::endl;
		// skip 1st "infinite" point
		for(++vit; vit != tds.vertices_end() ; ++vit)
		{
			V[vit] = inum++;
			p = vit->point();
			os << p.x() <<" "<< p.y() <<" 0" << std::endl;
		}
	}
	os << "# list of faces: number of points, followed by the sequence number of the composing points (indexed from zero)"<<endl;

	// vertices of the faces
	int dim = 2;
	
	for( TDS::Face_iterator ib = tds.face_iterator_base_begin(); ib != tds.face_iterator_base_end(); ++ib)
	{
		os <<"3";
		for(int j = 0; j < dim ; ++j)
		{
			os <<" "<<V[ib->vertex(j)];
			p = ib->vertex(j)->point();
			os <<"("<< p.x() <<" "<< p.y()<<")";
		}
		os << endl ;
	}
	os<<endl;
	os.close();
} // void saveTriangulationToOFF(char* filename, T* t)
