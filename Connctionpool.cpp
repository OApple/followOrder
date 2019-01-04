#include "Connctionpool.h"

#include <stdexcept>
#include <exception>
#include <stdio.h>



using namespace std;
using namespace sql;


/// 单例声明
/// 类内 class static 除了在class 之内声明，还需要在class 之外定义.
auto_ptr<CConnctionPool> CConnctionPool::auto_ptr_instance;

/**
 * @Synopsis  CConnctionPool
 *	连接池构造函数
 * @Param url
 * @Param userName
 * @Param password
 * @Param maxSize
 */
CConnctionPool::CConnctionPool()
{

}

/**
 * @Synopsis  ~CConnctionPool
 *	连接池析构函数
 */
CConnctionPool::~CConnctionPool(void)
{
    this->TerminateConnectionPool();
}

/**
 * @Synopsis  GetInstance
 *	获取连接池对象，单例模式
 * @Returns   连接池对象
 */
CConnctionPool& CConnctionPool::GetInstance(void)
{
    if (auto_ptr_instance.get() == 0) {
        auto_ptr_instance.reset(new CConnctionPool());
    }
    return *(auto_ptr_instance.get());
}

/**
 * @Synopsis  InitConnection
 *	初始化连接池，创建最大连接数的一半连接量
 * @Param iInitialSize
 */
void CConnctionPool::InitConnection(int iInitialSize)
{
    Connection *pConn;
    boost::mutex::scoped_lock lock(m_mut);
    for (int i = 0; i < iInitialSize; ++i) {
        pConn = this->CreateConnection();
        if (pConn) {
            m_connlist.push_back(pConn);
            ++(this->m_icurSize);
        } else {
            cout<<"创建Connection 出错!"<<endl;
        }
    }

}

/**
 * @Synopsis  CreateConnection
 *	创建连接，返回一个 Connection
 * @Returns   sql::connection*
 */
Connection* CConnctionPool::CreateConnection(void)
{
    Connection *pConn;
    try {
        pConn = m_pdriver->connect(this->m_surl, this->m_suserName, this->m_spassword);
        return pConn;
    }
    catch(sql::SQLException &e) {
        cout<<"创建连接出现错误!"<<endl;
        return NULL;
    }
    /*catch(sql::runtime_error  &e) {
        TP_LOG("运行时出现错误!");
        return NULL;
    }*/
}

/**
 * @Synopsis  GetConnection
 *	在连接池中获取一个连接
 * @Returns   sql::connection*
 */
Connection* CConnctionPool::GetConnection(void)
{
    Connection *pConn;
    boost::mutex::scoped_lock lock(m_mut);
    /// 连接池容器中还有连接
    if (m_connlist.size() > 0) {
        pConn = m_connlist.front();		/// 得到第一个连接
        m_connlist.pop_front();			/// 移除第一个连接
        /// 如果连接池已经关闭，删除后重新建立一个
        if (pConn->isClosed()) {
            delete pConn;
            pConn = this->CreateConnection();
        }
        /// 如果连接为空，则创建连接出错
        if (NULL == pConn) {
            --m_icurSize;
        }

        return pConn;
    } else {
        /// 还可以创建新的连接
        if (m_icurSize < m_imaxSize) {
            pConn = this->CreateConnection();
            if (pConn) {
                ++m_icurSize;

                return pConn;
            } else {

                return NULL;
            }
        } else {
            /// 建立的连接数已达到 m_imaxSize 上限

            return NULL;
        }
    }
}

/**
 * @Synopsis  ReleaseConnection
 *	回收数据库连接
 * @Param pConn
 */
void CConnctionPool::ReleaseConnection(Connection* pConn)
{
    if (pConn) {
        boost::mutex::scoped_lock lock(m_mut);
        m_connlist.push_back(pConn);

    }
}

/**
 * @Synopsis  TerminateConnectionPool
 *	销毁连接池，首先要先销毁连接池中的连接
 */
void CConnctionPool::TerminateConnectionPool(void)
{
    list<Connection*>::iterator iconn;
    boost::mutex::scoped_lock lock(m_mut);
    /// 为了符合泛型编程而使用 != 判断
    for (iconn=m_connlist.begin(); iconn != m_connlist.end(); ++iconn)
    {
        this->TerminateConnection(*iconn);	/// 销毁连接池中的连接
    }
    m_icurSize = 0;
    m_connlist.clear();		/// 清空连接池中的连接

}

/**
 * @Synopsis  TerminateConnection
 *	销毁一个连接
 * @Param pConn
 */
void CConnctionPool::TerminateConnection(Connection *pConn)
{
    if (pConn) {
        try{
            pConn->close();
        } catch(sql::SQLException &e) {
            //TP_LOG(e.what());
        }
        /*catch(sql::runtime_error  &e) {
            TP_LOG(e.what());
        }*/
    }
    delete pConn;
}

/**
 * @Synopsis  GetCurrentSize
 *	获取当前线程池中的有效的连接数(可选)
 * @Returns
 */
int CConnctionPool::GetCurrentSize(void)
{
    return m_icurSize;
}

/**
 * @brief CConnctionPool::initDBSetting
 * @param url
 * @param userName
 * @param password
 * @param maxSize
 *
 */
void CConnctionPool::initDBSetting(string url, string userName, string password, int maxSize)
{
    m_icurSize =0;
    m_imaxSize=maxSize;
    m_suserName = userName;
    m_spassword=password;
    m_surl=url;
    try {
        m_pdriver = get_driver_instance();
    }
    catch(sql::SQLException &e) {
        cout<<"驱动连接出错;\n"<<endl;
    }
    /*catch(sql::runtime_error &e) {
        TP_LOG("运行出错了\n");
    }*/
    this->InitConnection(m_imaxSize/2);
}
