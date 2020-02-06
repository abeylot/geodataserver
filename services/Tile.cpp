#include "Tile.hpp"
#include "renderers/SvgRenderer.hpp"
#include "../Coordinates.hpp"
#include "../common/constants.hpp"
#include <string>
#include <math.h>
#include <iostream>
#include "zlib.h"
#include "assert.h"
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
#define CHUNK 0x4000
//#include "renderers/ClcArea.hpp"

int def(std::string& source, FILE* dest)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    //ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    ret = deflateInit2(&strm,
                       Z_DEFAULT_COMPRESSION,
                       Z_DEFLATED,
                       16 + MAX_WBITS,
                       MAX_MEM_LEVEL,
                       Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        return ret;
    uint64_t pos = 0;
    uint64_t length = source.length();
    bool do_stop = false;

    /* compress until end of file */
    do {
		if((length - pos) > CHUNK)
		{
			strm.avail_in = CHUNK;
			memcpy(in, source.c_str() + pos, CHUNK);
			pos += CHUNK; 
		}
		else
		{
			strm.avail_in = length - pos;
			memcpy(in, source.c_str() + pos, length - pos);
			pos = length;
			do_stop = true; 			
		} 
        /*if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }*/
        flush = do_stop ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            //dest.append(out,have);
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}


int def(std::string& source, std::string& dest)
{
	dest = "";
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    //ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    ret = deflateInit2(&strm,
                       Z_DEFAULT_COMPRESSION,
                       Z_DEFLATED,
                       16 + MAX_WBITS,
                       MAX_MEM_LEVEL,
                       Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        return ret;
    uint64_t pos = 0;
    uint64_t length = source.length();
    bool do_stop = false;

    /* compress until end of file */
    do {
		if((length - pos) > CHUNK)
		{
			strm.avail_in = CHUNK;
			memcpy(in, source.c_str() + pos, CHUNK);
			pos += CHUNK; 
		}
		else
		{
			strm.avail_in = length - pos;
			memcpy(in, source.c_str() + pos, length - pos);
			pos = length;
			do_stop = true; 			
		} 
        /*if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }*/
        flush = do_stop ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            //dest.append(out,have);
            //if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
            //    (void)inflateEnd(&strm);
            //    return Z_ERRNO;
            //
            dest.append((const char*)out, have);
            //}
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}



/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
int inf(FILE *source, std::string& dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    //ret = inflateInit(&strm);
    ret = inflateInit2(&strm,16 + MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
           dest.append((char *)out,have);
           /*if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }*/
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}



double tilex2long(int x, int z)
{
    int div = 1 << z;
    double result =  (double)x / (double)div * 360.0 - 180;
    if (result >= 179.999) result = 179.999;
    return result;
}

double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

Msg* Tile::processRequest(Msg* request, CompiledDataManager& mger)
{
    char filename[250];
    /*std::string sLon1 = request->getRecord(2)->getNamedValue("longitude1");
    std::string sLat1 = request->getRecord(2)->getNamedValue("lattitude1");

    std::string sLon2 = request->getRecord(2)->getNamedValue("longitude2");
    std::string sLat2 = request->getRecord(2)->getNamedValue("lattitude2");*/

    double lon1, lon2, lat1, lat2;


    lon1 = tilex2long(_x, _z);
    lon2 = tilex2long(_x+1, _z);

    lat1 = tiley2lat(_y, _z);
    lat2 = tiley2lat(_y+1, _z);



    Rectangle rect;

    rect.x0 = Coordinates::toNormalizedLon(std::to_string(lon1));
    rect.y0 = Coordinates::toNormalizedLat(std::to_string(lat1));

    rect.x1 = Coordinates::toNormalizedLon(std::to_string(lon2));
    rect.y1 = Coordinates::toNormalizedLat(std::to_string(lat2));

    //rect = rect*1.2;


    std::cout << lon1 << "::" << lat1 << "::" << lon2 << "::" << lat2 << "\n";
    std::cout << rect.x0 << "::" << rect.y0 << "::" << rect.x1 << "::" << rect.y1 << "\n";


    /*uint32_t meany = (rect.y0 >> 1) + (rect.y1  >> 1);
    double angle = PI * ((meany *1.0) / (1.0 * UINT32_MAX));
    double ratio = 	( 2.0* sin(angle)*(rect.x1 - rect.x0))/(1.0*(rect.y1 - rect.y0));
    if (ratio < 0) ratio *= -1;


    if(ratio > 1) {
    		szx = 256;
    	    szy = 256.0 / ratio;
    } else {
    		szy = 256;
    	    szx = 256.0 * ratio;
    }*/

    uint32_t szx, szy;
    szx = szy = 256;


    std::string resp = "";

    Msg* rep = new Msg;
    HttpEncoder encoder;
    encoder.build200Header(rep, "image/svg+xml");
    std::string res = "";
    bool filefound = false;
    size_t len;
    FILE* in;
    FILE* out;
    if(_z <= _cachelevel)
    {
    	snprintf(filename,250,"cache/%ld/%ld/%ld.gz",_z,_x,_y);
        in = fopen(filename, "r");
        if(in != NULL)
        {
			filefound=true;
			rep->getRecord(0)->addBlock("HTTPEncoding=gzip");
			//inf(in, res);
		char buffer[4097];
		while( (len = fread(buffer,1,4096,in)) )
                {	buffer[len] = 0;
			res.append(buffer, len);
                }
        }
        
    }
    if(filefound)
    {
	printf("USING CACHE !\n");
        fclose(in);
        encoder.addContent(rep,res);
    }else{	
		std::string dir1 = "cache/" + std::to_string(_z);
		std::string dir2 = dir1 + "/" + std::to_string(_x);


    	SvgRenderer rdr(&mger,_z);
        std::string tag = "";
        res = rdr.renderItems(rect,szx,szy,tag);
        rep->getRecord(0)->addBlock("HTTPEncoding=gzip");
        std::string smallRes;
        def(res, smallRes);
        encoder.addContent(rep,smallRes);
        std::cout << "cache level " << _cachelevel << "\n";
	if(_z <= _cachelevel)
        {
			boost::filesystem::path p1(dir1);
			boost::filesystem::path p2(dir2);

			if(! boost::filesystem::exists(p1))
			{
				boost::filesystem::create_directory(p1);
			}

			if(! boost::filesystem::exists(p2))
			{
				boost::filesystem::create_directory(p2);
			}
	        out = fopen(filename, "w");
                if(out != NULL)
                {
                     /*len=fwrite(res.c_str(),res.length(),1,out);
                     fclose(out);*/
                    def(res, out);
                    fclose(out);
                }
                else
                {
					printf("UNABLE TO CACHE %s !\n", filename);
				}
        }
    }
    return rep;

}

