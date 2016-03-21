<?php
namespace Home\Controller;
use Think\Controller;
class SearchController extends Controller {
    public function index(){
       
    	//搜索页面
        /*
        $url_prefix=__ROOT__."/index.php?c=page&a=index&id=";
    	$job_list=array(array("title"=>"java R & D高级研发工程师",
    		"logo"=>__ROOT__."/Uploads/Picture/2015-01-01/baidu.jpg",
    		"redirect"=> $url_prefix."1",
    		'company'=>'百度科技有限公司',
    		'salary'=>'20-30万',
    		'location'=>'北京',
    		'pub_tm'=>'10分钟前',
            'department'=>'联盟研发部',
            'description'=>'该职位符合你的要求么，就赶紧来学些吧',
    		'label'=>array('0'=>'发展空间大','1'=>'五险一金','2'=>'扁平管理')),
        array("title"=>"java R & D高级研发工程师",
            "logo"=>__ROOT__."/Uploads/Picture/2015-01-01/baidu.jpg",
            "redirect"=> "#",
            'company'=>'百度科技有限公司',
            'salary'=>'20-30万',
            'location'=>'北京',
            'pub_tm'=>'10分钟前',
            'department'=>'联盟研发部',
            'description'=>'该职位符合你的要求么，就赶紧来学些吧',
            'label'=>array('0'=>'发展空间大','1'=>'五险一金','2'=>'扁平管理'))
    	);
        */
        $job_list = $this->get_jobs(I('keywords'));
    	$this->assign('search_result',$job_list);
        $this->assign('title','翘课网，让找工作变得简单-职位搜索：'.I('keywords'));
        $this->assign('keywords','翘课网,找工作,在线教育,招聘,求职');
        $this->assign('description','翘课网，让找工作变得简单');
        $this->assign('head_title','关于 &quot; '.I('keywords') .' &quot; 的搜索职位结果');
        $this->display();
    }
    private function get_jobs($keywords) {

        //请求后台服务进行职位搜索
        $query['title'] = array('like','%'.$keywords.'%');
        $job_list = M('job_detail')->where($query)->select();
        
        foreach($job_list as $key=>$value) {
            $job_list[$key]['label'] = explode('|', $value['label']);
            $job_list[$key]['logo'] = __ROOT__.$value['logo'];
            $job_list[$key]['redirect'] = __ROOT__."/index.php?c=page&a=index&id=".$value['id'];
        }
        return $job_list;
    }
}